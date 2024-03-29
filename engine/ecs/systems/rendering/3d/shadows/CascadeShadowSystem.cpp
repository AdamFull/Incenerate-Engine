#include "CascadeShadowSystem.h"

#include "Engine.h"
#include "Helpers.h"

#include "ecs/components/components.h"

using namespace engine::graphics;
using namespace engine::ecs;

constexpr const float cascadeSplitLambda = 0.985f;
constexpr const float cascadeSplitOverlap = 0.9f;

void CCascadeShadowSystem::__create()
{
	FShaderCreateInfo specials;
	specials.pipeline_stage = "cascade_shadow";
	specials.vertex_type = EVertexType::eDefault;
	specials.cull_mode = vk::CullModeFlagBits::eFront;
	specials.front_face = vk::FrontFace::eCounterClockwise;
	specials.depth_test = true;
	specials.usages = 64;

	// TODO: do it using constants
	//specials.defines = { {"INVOCATION_COUNT", std::to_string(CASCADE_SHADOW_MAP_CASCADE_COUNT)} };
	shader_id = graphics->addShader("shadows:cascaded", specials);

	CBaseGraphicsSystem::__create();
}

void CCascadeShadowSystem::__update(float fDt)
{
	auto& shadowManager = EGEngine->getShadows();

	const auto* camera = EGEngine->getActiveCamera();

	if (!camera)
		return;

	std::array<std::array<FFrustum, CASCADE_SHADOW_MAP_CASCADE_COUNT>, MAX_DIRECTIONAL_LIGHT_SHADOW_COUNT> cadcaded_frustums;

	// Begin calculating cascades
	const auto& nearClip = camera->nearPlane;
	const auto& farClip = camera->farPlane;
	float clipRange = farClip - nearClip;

	float minZ = nearClip;
	float maxZ = nearClip + clipRange;

	float range = maxZ - minZ;
	float ratio = maxZ / minZ;

	uint32_t shadowIndex{ 0u };
	auto lights = registry->view<FTransformComponent, FDirectionalLightComponent>();
	for (auto [entity, ltransform, light] : lights.each())
	{
		// Reset shadow index
		light.shadowIndex = -1;

		// Calculate shadows
		std::array<float, CASCADE_SHADOW_MAP_CASCADE_COUNT> cascadeSplits;
		if (light.castShadows && shadowIndex < MAX_DIRECTIONAL_LIGHT_SHADOW_COUNT)
		{
			auto& shadow_commit = shadowManager->getCascadedShadowCommit(shadowIndex);

			// Calculate split depths based on view camera frustum
			// Based on method presented in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
			for (uint32_t i = 0; i < CASCADE_SHADOW_MAP_CASCADE_COUNT; i++)
			{
				float p = static_cast<float>(i + 1) / static_cast<float>(CASCADE_SHADOW_MAP_CASCADE_COUNT);
				float log = minZ * glm::pow(ratio, p);
				float uniform = minZ + range * p;
				float d = cascadeSplitLambda * (log - uniform) + uniform;
				cascadeSplits[i] = (d - nearClip) / clipRange;
			}

			// Calculate orthographic projection matrix for each cascade
			float lastSplitDist = 0.0;
			for (uint32_t i = 0; i < CASCADE_SHADOW_MAP_CASCADE_COUNT; i++)
			{
				auto& splitDist = cascadeSplits[i];

				std::array<glm::vec4, 8> frustumCorners =
				{
					//Near face
					glm::vec4{  1.0f,  1.0f, -1.0f, 1.0f },
					glm::vec4{ -1.0f,  1.0f, -1.0f, 1.0f },
					glm::vec4{  1.0f, -1.0f, -1.0f, 1.0f },
					glm::vec4{ -1.0f, -1.0f, -1.0f, 1.0f },

					//Far face
					glm::vec4{  1.0f,  1.0f, 1.0f, 1.0f },
					glm::vec4{ -1.0f,  1.0f, 1.0f, 1.0f },
					glm::vec4{  1.0f, -1.0f, 1.0f, 1.0f },
					glm::vec4{ -1.0f, -1.0f, 1.0f, 1.0f },
				};

				// Project frustum corners into world space
				auto projection = camera->projection;
				glm::mat4 invCam = glm::inverse(camera->projection * camera->view);
				for (auto& corner : frustumCorners)
				{
					glm::vec4 invCorner = invCam * corner;
					corner = invCorner / invCorner.w;
				}

				for (uint32_t j = 0; j < 4; ++j)
				{
					glm::vec4 dist = frustumCorners[j + 4] - frustumCorners[j];
					frustumCorners[j + 4] = frustumCorners[j] + (dist * splitDist);
					frustumCorners[j] = frustumCorners[j] + (dist * lastSplitDist);
				}

				// Get frustum center
				glm::vec3 frustumCenter = glm::vec3(0.0f);
				for (auto& corner : frustumCorners)
					frustumCenter += glm::vec3(corner);
				frustumCenter /= frustumCorners.size();

				float radius = 0.0f;
				for (auto& corner : frustumCorners)
				{
					float distance = glm::length(glm::vec3(corner) - frustumCenter);
					radius = glm::max(radius, distance);
				}
				radius = glm::ceil(radius * 16.0f) / 16.0f;

				glm::vec3 maxExtents = glm::vec3(radius);
				glm::vec3 minExtents = -maxExtents;

				glm::vec3 lightDir = glm::normalize(glm::toQuat(ltransform.model) * glm::vec3(0.f, 0.f, 1.f));
				glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter, camera->up);
				glm::mat4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, minExtents.z, maxExtents.z - minExtents.z);

				auto shadowMatrix = lightOrthoMatrix * lightViewMatrix;
				glm::vec4 shadowOrigin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
				shadowOrigin = shadowMatrix * shadowOrigin;
				float storedW = shadowOrigin.w;
				shadowOrigin = shadowOrigin * static_cast<float>(CASCADE_SHADOW_MAP_RESOLUTION) / 2.0f;
				glm::vec4 roundedOrigin = glm::round(shadowOrigin);
				glm::vec4 roundOffset = roundedOrigin - shadowOrigin;
				roundOffset = roundOffset * 2.0f / static_cast<float>(CASCADE_SHADOW_MAP_RESOLUTION);
				roundOffset.z = 0.0f;
				roundOffset.w = 0.0f;
				glm::mat4 lightProjectionMatrix = lightOrthoMatrix;
				lightProjectionMatrix[3] += roundOffset;

				// Store split distance and matrix in cascade
				shadow_commit.farClip = farClip;
				shadow_commit.splitDepths[i] = (nearClip + splitDist * clipRange) * -1.0f;
				shadow_commit.cascadeViewProj[i] = lightProjectionMatrix * lightViewMatrix;

				cadcaded_frustums[shadowIndex][i].update(lightViewMatrix, lightProjectionMatrix);

				lastSplitDist = cascadeSplits[i];
			}

			// Set shadow index and increment it
			light.shadowIndex = shadowIndex;
			shadowIndex++;
		}
	}

	// Render shadow cascades
	auto stage = graphics->getRenderStageID("cascade_shadow");
	graphics->bindRenderer(stage);

	graphics->bindShader(shader_id);
	graphics->setManualShaderControlFlag(true);

	auto& pPush = graphics->getPushBlockHandle("shadowData");

	auto meshes = registry->view<FTransformComponent, FMeshComponent>();
	for (auto [entity, mtransform, mesh] : meshes.each())
	{
		auto& head = registry->get<FSceneComponent>(mesh.head);

		if (!graphics->bindVertexBuffer(mesh.vbo_id))
			continue;

		bool bHasSkin{ mesh.skin > -1 };

		pPush->set("hasSkin", static_cast<int32_t>(bHasSkin));
		pPush->set("model", mtransform.model);

		if (bHasSkin)
		{
			auto& pInstanceUBO = graphics->getUniformHandle("UBOInstancing");
			if (pInstanceUBO)
				pInstanceUBO->set("instances", mesh.vInstances);

			auto& pJoints = graphics->getUniformHandle("FSkinning");
			if (pJoints)
			{
				auto& skin = head.skins[mesh.skin];
				pJoints->set("jointMatrices", skin.jointMatrices);
			}
		}

		graphics->flushShader();

		for (auto& meshlet : mesh.vMeshlets)
		{
			for (size_t shadow = 0ull; shadow < shadowIndex; ++shadow)
			{
				auto& shadow_commit = shadowManager->getCascadedShadowCommit(shadow);
				for (size_t cascade = 0ull; cascade < CASCADE_SHADOW_MAP_CASCADE_COUNT; ++cascade)
				{
					auto& frustum = cadcaded_frustums[shadow][cascade];

					glm::vec4 clipSpacePos = shadow_commit.cascadeViewProj[cascade] * mtransform.model * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
					float clipSpaceZ = clipSpacePos.z / clipSpacePos.w;
					float worldZ = camera->nearPlane + (camera->farPlane - camera->nearPlane) * ((clipSpaceZ + 1.0f) / 2.0f);

					auto inLightView = head.castShadows && frustum.checkBox(mtransform.rposition + meshlet.dimensions.min * mtransform.rscale, mtransform.rposition + meshlet.dimensions.max * mtransform.rscale);
					if (head.castShadows && inLightView)
					{
						// Calculating level of detail for cascade
						auto lod_level = rangeToRange<uint32_t>(cascade, 0u, CASCADE_SHADOW_MAP_CASCADE_COUNT - 1u, 1u, MAX_LEVEL_OF_DETAIL - 1u);
						auto& lod = meshlet.levels_of_detail[lod_level];

						//Rendering shadow map
						pPush->set("viewProjMat", shadow_commit.cascadeViewProj[cascade]);
						pPush->set("layer", cascade);
						graphics->flushConstantRanges(pPush);

						graphics->draw(lod.begin_vertex, lod.vertex_count, lod.begin_index, lod.index_count, mesh.instanceCount == 0 ? 1 : mesh.instanceCount);
					}
				}
			}
		}
	}

	graphics->setManualShaderControlFlag(false);
	graphics->bindShader(invalid_index);
	graphics->bindVertexBuffer(invalid_index);

	graphics->bindRenderer(invalid_index);
}