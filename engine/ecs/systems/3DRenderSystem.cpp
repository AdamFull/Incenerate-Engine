#include "3DRenderSystem.h"

#include "Engine.h"

#include "ecs/components/TransformComponent.h"
#include "ecs/components/MeshComponent.h"
#include "ecs/components/CameraComponent.h"

using namespace engine::graphics;
using namespace engine::ecs;

C3DRenderSystem::C3DRenderSystem()
{
}

void C3DRenderSystem::__create()
{
	
}

void C3DRenderSystem::__update(float fDt)
{
	auto& stage = EGGraphics->getRenderStage("deferred");
	auto commandBuffer = EGGraphics->getCommandBuffer();

	auto activeCamera = EGCoordinator->getActiveCamera();

	if (activeCamera)
	{
		auto& camera = EGCoordinator->getComponent<FCameraComponent>(activeCamera.value());
		auto& cameraTransform = EGCoordinator->getComponent <FTransformComponent>(activeCamera.value());

		for (auto& entity : mEntities)
		{
			FTransformComponent transform;
			auto& mesh = EGCoordinator->getComponent<FMeshComponent>(entity);
			auto& vbo = EGGraphics->getVertexBuffer(mesh.vbo_id);

			auto relatives = EGSceneGraph->relative(entity);
			while (!relatives.empty())
			{
				auto& front = relatives.top();
				auto& rt = EGCoordinator->getComponent<FTransformComponent>(front);
				transform += rt;
				relatives.pop();
			}

			vbo->bind(commandBuffer);

			for (auto& meshlet : mesh.vMeshlets)
			{
				bool needToRender{ true };
				//needToRender = camera.frustum.checkBox(transform.position + meshlet.dimensions.min * transform.scale, transform.position + meshlet.dimensions.max * transform.scale);
				meshlet.bWasCulled = needToRender;

				if (needToRender)
				{
					auto& pMaterial = EGGraphics->getMaterial(meshlet.material);
					if (pMaterial)
					{
						auto& params = pMaterial->getParameters();
						auto& pShader = EGGraphics->getShader(pMaterial->getShader());
						if (pShader)
						{
							auto model = transform.getModel();
							auto normal = glm::transpose(glm::inverse(model));

							auto& pUBO = pShader->getUniformBuffer("FUniformData");
							pUBO->set("model", model);
							pUBO->set("view", camera.view);
							pUBO->set("projection", camera.projection);
							pUBO->set("normal", normal);
							pUBO->set("viewDir", cameraTransform.position);
							pUBO->set("viewportDim", EGGraphics->getDevice()->getExtent());
							pUBO->set("frustumPlanes", camera.frustum.getFrustumSides());

							auto& pSurface = pShader->getUniformBuffer("UBOMaterial");
							if (pSurface)
							{
								pSurface->set("baseColorFactor", params.baseColorFactor);
								pSurface->set("emissiveFactor", params.emissiveFactor);
								pSurface->set("alphaCutoff", params.alphaCutoff);
								pSurface->set("normalScale", params.normalScale);
								pSurface->set("occlusionStrenth", params.occlusionStrenth);
								pSurface->set("metallicFactor", params.metallicFactor);
								pSurface->set("roughnessFactor", params.roughnessFactor);
								pSurface->set("tessellationFactor", params.tessellationFactor);
								pSurface->set("tessellationStrength", params.tessStrength);
							}

							for (auto& [name, id] : pMaterial->getTextures())
								pShader->addTexture(name, id);

							pShader->predraw(commandBuffer);

							commandBuffer.drawIndexed(meshlet.index_count, 1, meshlet.begin_index, 0, 0);
							continue;
						}
					}
				}

			}
		}

		stage->end(commandBuffer);
	}
}