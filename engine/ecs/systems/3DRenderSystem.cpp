#include "3DRenderSystem.h"

#include "Engine.h"

#include "ecs/components/TransformComponent.h"
#include "ecs/components/MeshComponent.h"

using namespace engine::graphics;
using namespace engine::ecs;

C3DRenderSystem::C3DRenderSystem()
{
	EGCoordinator->addEventListener(Events::Graphics::ReCreate, this, &C3DRenderSystem::onReCreate);
}

void C3DRenderSystem::__create()
{
	stageCI.srName = "deferred";
	stageCI.viewport.offset = vk::Offset2D(0, 0);
	stageCI.viewport.extent = EGGraphics->getDevice()->getExtent();
	stageCI.bFlipViewport = true;
	stageCI.vImages.emplace_back(FCIImage{"packed_tex", vk::Format::eR32G32B32A32Uint, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled });
	stageCI.vImages.emplace_back(FCIImage{"emission_tex", vk::Format::eB10G11R11UfloatPack32, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled });
	stageCI.vImages.emplace_back(FCIImage{"depth_tex", EGGraphics->getDevice()->getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled });
	stageCI.vOutputs.emplace_back("packed_tex");
	stageCI.vOutputs.emplace_back("emission_tex");
	stageCI.vDescriptions.emplace_back("depth_tex");
	stageCI.vDependencies.emplace_back(
		FCIDependency(
			FCIDependencyDesc(
				VK_SUBPASS_EXTERNAL,
				vk::PipelineStageFlagBits::eFragmentShader,
				vk::AccessFlagBits::eShaderRead
			),
			FCIDependencyDesc(
				0,
				vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
				vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite
			)
		)
	);
	stageCI.vDependencies.emplace_back(
		FCIDependency(
			FCIDependencyDesc(
				0,
				vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests,
				vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite
			),
			FCIDependencyDesc(
				VK_SUBPASS_EXTERNAL,
				vk::PipelineStageFlagBits::eColorAttachmentOutput,
				vk::AccessFlagBits::eColorAttachmentWrite
			)
		)
	);


	stageId = EGGraphics->createRenderStage("deferred");
	auto& pStage = EGGraphics->getRenderStage(stageId);
	pStage->create(stageCI);
}

void C3DRenderSystem::__update(float fDt)
{
	auto& stage = EGGraphics->getRenderStage(stageId);
	auto commandBuffer = EGGraphics->begin();
	
	stage->begin(commandBuffer);

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
			// Culling here

			if (needToRender)
			{
				auto& pMaterial = EGGraphics->getMaterial(meshlet.material);
				if (pMaterial)
				{
					auto& params = pMaterial->getParameters();
					auto& pShader = EGGraphics->getShader(pMaterial->getShader());
					if (pShader)
					{
						auto& pUBO = pShader->getUniformBuffer("FUniformData");
						pUBO->set("model", transform.getModel());
						//pUBO->set("view", );
						//pUBO->set("projection", );
						//pUBO->set("normal", );
						//pUBO->set("viewDir", );
						pUBO->set("viewportDim", EGGraphics->getDevice()->getExtent());
						//pUBO->set("frustumPlanes", );

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

						if (params.albedo != invalid_index)
							pShader->addTexture("color_tex", params.albedo);

						if (params.metallicRoughness != invalid_index)
							pShader->addTexture("rmah_tex", params.metallicRoughness);

						if (params.normalMap != invalid_index)
							pShader->addTexture("normal_tex", params.normalMap);

						if (params.ambientOcclusion != invalid_index)
							pShader->addTexture("occlusion_tex", params.ambientOcclusion);

						if (params.heightMap != invalid_index)
							pShader->addTexture("height_tex", params.heightMap);

						if (params.emissionColor != invalid_index)
							pShader->addTexture("emissive_tex", params.emissionColor);

						pShader->predraw(commandBuffer);

						commandBuffer.drawIndexed(meshlet.index_count, 1, meshlet.begin_index, 0, 0);
					}
				}
			}

		}
	}

	stage->end(commandBuffer);

	EGGraphics->end();
}

void C3DRenderSystem::onReCreate(CEvent& event)
{
	stageCI.viewport.extent = EGGraphics->getDevice()->getExtent();
	auto& stage = EGGraphics->getRenderStage(stageId);
	stage->reCreate(stageCI);
}