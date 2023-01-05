#include "3DRenderSystem.h"

#include "Engine.h"

#include "ecs/components/TransformComponent.h"
#include "ecs/components/MeshComponent.h"

using namespace engine::loaders;
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
	stageCI.vImages.emplace_back(FCIImage{"present_khr", EGGraphics->getDevice()->getImageFormat(), vk::ImageUsageFlagBits::eColorAttachment });
	stageCI.vImages.emplace_back(FCIImage{"depth_tex", EGGraphics->getDevice()->getDepthFormat(), vk::ImageUsageFlagBits::eDepthStencilAttachment });
	stageCI.vOutputs.emplace_back("present_khr");
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
		auto& transform = EGCoordinator->getComponent<FTransformComponent>(entity);
		auto& mesh = EGCoordinator->getComponent<FMeshComponent>(entity);
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