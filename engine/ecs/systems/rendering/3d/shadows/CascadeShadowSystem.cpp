#include "CascadeShadowSystem.h"

#include "Engine.h"

#include "ecs/components/components.h"

using namespace engine::graphics;
using namespace engine::ecs;

const float cascadeSplitLambda = 0.985f;

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
				cascadeSplits[i] = (d - nearClip) / clipRange;
			}

			// Calculate orthographic projection matrix for each cascade
			float lastSplitDist = 0.0;
			for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++)
			{
				auto& splitDist = cascadeSplits[i];

				glm::vec3 frustumCorners[8] =
				{
					glm::vec3(-1.0f,  1.0f, -1.0f),
					glm::vec3(1.0f,  1.0f, -1.0f),
					glm::vec3(1.0f, -1.0f, -1.0f),
					glm::vec3(-1.0f, -1.0f, -1.0f),
					glm::vec3(-1.0f,  1.0f,  1.0f),
					glm::vec3(1.0f,  1.0f,  1.0f),
					glm::vec3(1.0f, -1.0f,  1.0f),
					glm::vec3(-1.0f, -1.0f,  1.0f),
				};

				// Project frustum corners into world space
				glm::mat4 invCam = glm::inverse(camera->projection * camera->view);
				for (uint32_t j = 0; j < 8; ++j)
				{
					glm::vec4 invCorner = invCam * glm::vec4(frustumCorners[j], 1.0f);
					frustumCorners[j] = invCorner / invCorner.w;
				}

				for (uint32_t j = 0; j < 4; ++j)
				{
					glm::vec3 dist = frustumCorners[j + 4] - frustumCorners[j];
					frustumCorners[j + 4] = frustumCorners[j] + (dist * splitDist);
					frustumCorners[j] = frustumCorners[j] + (dist * lastSplitDist);
				}

				// Get frustum center
				glm::vec3 frustumCenter = glm::vec3(0.0f);
				for (uint32_t j = 0; j < 8; ++j)
					frustumCenter += frustumCorners[j];

				frustumCenter /= 8.0f;

				float radius = 0.0f;
				for (uint32_t j = 0; j < 8; ++j) 
				{
					float distance = glm::length(frustumCorners[j] - frustumCenter);
					radius = glm::max(radius, distance);
				}
				radius = glm::ceil(radius * 16.0f) / 16.0f;

				glm::vec3 maxExtents = glm::vec3(radius);
				glm::vec3 minExtents = -maxExtents;

				//glm::vec3 lightDir = glm::normalize(ltransform.model[2]);
				//glm::vec3 lightDir = glm::normalize((glm::toQuat(ltransform.model) * glm::vec3(0.f, 0.f, 1.f)));
				glm::vec3 lightDir = { 1.f, -1.f, 1.f };
				glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter + -lightDir * -minExtents.z, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));
				glm::mat4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, maxExtents.z - minExtents.z);

				// Store split distance and matrix in cascade
				light.splitDepths[i] = (nearClip + splitDist * clipRange) * -1.0f;
				light.cascadeViewProj[i] = lightOrthoMatrix * lightViewMatrix;

				lastSplitDist = cascadeSplits[i];
			}

			graphics->bindShader(shader_id);
			graphics->setManualShaderControlFlag(true);

			auto& pUniform = graphics->getUniformHandle("UBOShadowmap");
			pUniform->set("viewProjMat", light.cascadeViewProj);
			graphics->flushShader();

			auto meshes = registry->view<FTransformComponent, FMeshComponent>();
			for (auto [entity, mtransform, mesh] : meshes.each())
			{
				auto& head = registry->get<FSceneComponent>(mesh.head);

				if (!graphics->bindVertexBuffer(mesh.vbo_id))
					continue;

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