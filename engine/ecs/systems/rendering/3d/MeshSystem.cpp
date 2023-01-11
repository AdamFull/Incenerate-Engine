#include "MeshSystem.h"


#include "Engine.h"

#include "ecs/components/TransformComponent.h"
#include "ecs/components/MeshComponent.h"
#include "ecs/components/CameraComponent.h"
#include "ecs/helper.hpp"

using namespace engine::graphics;
using namespace engine::ecs;

void CMeshSystem::__create()
{

}

void CMeshSystem::__update(float fDt)
{
	auto& registry = EGCoordinator;

	auto commandBuffer = EGGraphics->getCommandBuffer();

	auto ecamera = get_active_camera(registry);
	auto& camera = registry.get<FCameraComponent>(ecamera);
	auto& cameraTransform = registry.get<FTransformComponent>(ecamera);

	auto view = registry.view<FTransformComponent, FMeshComponent>();
	for (auto [entity, transform, mesh] : view.each())
	{
		auto& vbo = EGGraphics->getVertexBuffer(mesh.vbo_id);

		vbo->bind(commandBuffer);

		for (auto& meshlet : mesh.vMeshlets)
		{
			bool needToRender{ true };
			needToRender = camera.frustum.checkBox(transform.rposition + meshlet.dimensions.min * transform.rscale, transform.rposition + meshlet.dimensions.max * transform.rscale);
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
						auto normal = glm::transpose(glm::inverse(transform.model));

						auto& pUBO = pShader->getUniformBuffer("FUniformData");
						pUBO->set("model", transform.model);
						pUBO->set("view", camera.view);
						pUBO->set("projection", camera.projection);
						pUBO->set("normal", normal);
						pUBO->set("viewDir", cameraTransform.rposition);
						pUBO->set("viewportDim", EGGraphics->getDevice()->getExtent(true));
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
}