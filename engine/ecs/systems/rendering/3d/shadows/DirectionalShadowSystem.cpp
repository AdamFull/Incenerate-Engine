#include "DirectionalShadowSystem.h"

#include "Engine.h"

#include "ecs/components/components.h"

using namespace engine::graphics;
using namespace engine::ecs;

void CDirectionalShadowSystem::__create()
{
	FShaderCreateInfo specials;
	specials.pipeline_stage = "direct_shadow";
	specials.vertex_type = EVertexType::eDefault;
	specials.cull_mode = vk::CullModeFlagBits::eFront;
	specials.front_face = vk::FrontFace::eCounterClockwise;
	specials.depth_test = true;
	specials.usages = MAX_SPOT_LIGHT_SHADOW_COUNT * 128;
	//specials.defines = { {"INVOCATION_COUNT", std::to_string(MAX_SPOT_LIGHT_COUNT)} };
	shader_id = graphics->addShader("shadows:directional", specials);

	CBaseGraphicsSystem::__create();
}

void CDirectionalShadowSystem::__update(float fDt)
{
	auto& shadowManager = EGEngine->getShadows();

	const auto* camera = EGEngine->getActiveCamera();

	if (!camera)
		return;

	auto stage = graphics->getRenderStageID("direct_shadow");
	graphics->bindRenderer(stage);

	// Calculating spot light matrices
	{
		uint32_t lightStride{ 0 };
		auto lights = registry->view<FTransformComponent, FSpotLightComponent>();
		for (auto [entity, ltransform, light] : lights.each())
		{
			light.shadowIndex = -1;

			if (!light.castShadows && lightStride < MAX_SPOT_LIGHT_SHADOW_COUNT)
				continue;

			auto& shadow_commit = shadowManager->getSpotShadowCommit(lightStride);

			FFrustum frustum;
			const glm::vec3 dp(0.0000001f);

			auto lightDir = light.toTarget ? light.target : glm::normalize(glm::toQuat(ltransform.model) * glm::vec3(0.f, 0.f, 1.f));
			glm::mat4 shadowProj = glm::perspective(light.outerAngle * 2.f, 1.0f, 0.1f, 64.f);
			glm::mat4 shadowView = glm::lookAt(ltransform.rposition, ltransform.position + lightDir, glm::vec3(0.0f, 1.0f, 0.0f));
			shadow_commit.shadowView = shadowProj * shadowView;

			frustum.update(shadowView, shadowProj);

			graphics->bindShader(shader_id);
			graphics->setManualShaderControlFlag(true);

			auto& pPush = graphics->getPushBlockHandle("shadowData");
			pPush->set("viewProjMat", shadow_commit.shadowView);
			pPush->set("layer", lightStride);

			auto distance = glm::distance(camera->viewPos, ltransform.rposition);
			auto lod_level = getLodLevel(camera->nearPlane, camera->farPlane, distance);

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
					if (pJoints && bHasSkin)
					{
						auto& skin = head.skins[mesh.skin];
						pJoints->set("jointMatrices", skin.jointMatrices);
					}
				}

				graphics->flushShader();

				for (auto& meshlet : mesh.vMeshlets)
				{
					auto inLightView = head.castShadows && frustum.checkBox(mtransform.rposition + meshlet.dimensions.min * mtransform.rscale, mtransform.rposition + meshlet.dimensions.max * mtransform.rscale);

					if (inLightView)
					{
						auto& lod = meshlet.levels_of_detail[lod_level];
						graphics->draw(lod.begin_vertex, lod.vertex_count, lod.begin_index, lod.index_count);
					}
				}

				graphics->bindVertexBuffer(invalid_index);
			}

			graphics->setManualShaderControlFlag(false);
			graphics->bindShader(shader_id);

			// Set shadow index and increment it
			shadow_commit.index = light.shadowIndex = lightStride;
			lightStride++;
		}
	}

	graphics->bindRenderer(invalid_index);
}