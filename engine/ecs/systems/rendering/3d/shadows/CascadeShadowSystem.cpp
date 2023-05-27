#include "CascadeShadowSystem.h"

#include "Engine.h"

#include "ecs/components/components.h"

using namespace engine::graphics;
using namespace engine::ecs;

constexpr const float cascadeSplitLambda = 0.985f;
constexpr const float cascadeSplitOverlap = 0.1f;

void CCascadeShadowSystem::__create()
{
	FShaderSpecials specials;
	specials.defines = { {"INVOCATION_COUNT", std::to_string(SHADOW_MAP_CASCADE_COUNT)} };
	shader_id = graphics->addShader("cascade_shadow_pass", specials);

	CBaseGraphicsSystem::__create();
}

void CCascadeShadowSystem::__update(float fDt)
{
	const auto* camera = EGEngine->getActiveCamera();

	if (!camera)
		return;

	const auto& nearClip = camera->nearPlane;
	const auto& farClip = camera->farPlane;
	float clipRange = farClip - nearClip;

	float minZ = nearClip;
	float maxZ = nearClip + clipRange;

	float range = maxZ - minZ;
	float ratio = maxZ / minZ;

	auto stage = graphics->getRenderStageID("cascade_shadow");
	graphics->bindRenderer(stage);

	auto lights = registry->view<FTransformComponent, FDirectionalLightComponent>();
	for (auto [entity, ltransform, light] : lights.each())
	{
		std::array<float, SHADOW_MAP_CASCADE_COUNT> cascadeSplits;
		if (light.castShadows)
		{
			// Calculate split depths based on view camera frustum
			// Based on method presented in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
			for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
			{
				float p = (i + 1) / static_cast<float>(SHADOW_MAP_CASCADE_COUNT);
				float log = minZ * glm::pow(ratio, p);
				float uniform = minZ + range * p;
				float d = cascadeSplitLambda * (log - uniform) + uniform;

				if (i != 0)
					d += cascadeSplitOverlap * (d - cascadeSplits[i - 1]);

				cascadeSplits[i] = (d - nearClip) / clipRange;
			}

			// Calculate orthographic projection matrix for each cascade
			float lastSplitDist = 0.0;
			for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
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
				glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));
				glm::mat4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, -15.0f, maxExtents.z - minExtents.z);

				auto shadowMatrix = lightOrthoMatrix * lightViewMatrix;
				glm::vec4 shadowOrigin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
				shadowOrigin = shadowMatrix * shadowOrigin;
				float storedW = shadowOrigin.w;
				shadowOrigin = shadowOrigin * static_cast<float>(SHADOW_MAP_RESOLUTION) / 2.0f;
				glm::vec4 roundedOrigin = glm::round(shadowOrigin);
				glm::vec4 roundOffset = roundedOrigin - shadowOrigin;
				roundOffset = roundOffset * 2.0f / static_cast<float>(SHADOW_MAP_RESOLUTION);
				roundOffset.z = 0.0f;
				roundOffset.w = 0.0f;
				glm::mat4 lightProjectionMatrix = lightOrthoMatrix;
				lightProjectionMatrix[3] += roundOffset;

				// Store split distance and matrix in cascade
				light.splitDepths[i] = (nearClip + splitDist * clipRange) * -1.0f;
				light.cascadeViewProj[i] = lightProjectionMatrix * lightViewMatrix;

				lastSplitDist = cascadeSplits[i];
			}



			graphics->bindShader(shader_id);
			graphics->setManualShaderControlFlag(true);

			auto& pUniform = graphics->getUniformHandle("UBOShadowmap");
			pUniform->set("viewProjMat", light.cascadeViewProj);
			graphics->flushShader();

			static std::array<glm::mat4, 128> joints{ glm::mat4(1.f) };

			auto meshes = registry->view<FTransformComponent, FMeshComponent>();
			for (auto [entity, mtransform, mesh] : meshes.each())
			{
				auto& head = registry->get<FSceneComponent>(mesh.head);

				if (!graphics->bindVertexBuffer(mesh.vbo_id))
					continue;

				bool bHasSkin{ false };
				entt::entity armature{ entt::null };
				if (mesh.skin > -1)
				{
					bHasSkin = true;
					auto& scene = registry->get<FSceneComponent>(mesh.head);
					auto invTransform = glm::inverse(mtransform.model);
					auto& skin = scene.skins[mesh.skin];
					armature = skin.root;

					for (uint32_t i = 0; i < skin.joints.size(); i++)
					{
						auto& jtransform = registry->get<FTransformComponent>(skin.joints[i]);
						joints[i] = jtransform.model * skin.inverseBindMatrices[i];
						joints[i] = invTransform * joints[i];
					}
				}

				// TODO: Add skinning support
				for (auto& meshlet : mesh.vMeshlets)
				{
					//auto inLightView = frustum.checkBox(mtransform.rposition + meshlet.dimensions.min * mtransform.rscale, mtransform.rposition + meshlet.dimensions.max * mtransform.rscale);

					if (head.castShadows)
					{
						auto& pPush = graphics->getPushBlockHandle("modelData");
						pPush->set("model", mtransform.model);
						graphics->flushConstantRanges(pPush);

						graphics->draw(meshlet.begin_vertex, meshlet.vertex_count, meshlet.begin_index, meshlet.index_count);
					}
				}

				graphics->bindVertexBuffer(invalid_index);
			}

			graphics->setManualShaderControlFlag(false);
			graphics->bindShader(invalid_index);
		}
	}

	graphics->bindRenderer(invalid_index);
}