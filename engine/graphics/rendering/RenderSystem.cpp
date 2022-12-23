#include "RenderSystem.h"
#include "RenderSystemParser.h"
#include "APIHandle.h"

using namespace engine;
using namespace engine::graphics;

CRenderSystem::CRenderSystem(CDevice* device)
{
	pDevice = device;
	pParser = std::make_unique<CRenderSystemParser>(pDevice);
}

void CRenderSystem::create(const std::string& description)
{
	auto createInfo = pParser->parse(description);

	for (auto& stage : createInfo.vStages)
	{
		auto pStage = std::make_unique<CRenderStage>(pDevice);
		mStages.emplace(stage.srName, std::move(pStage));
		mStages[stage.srName]->create(stage);
	}

	image = pDevice->getAPI()->createImage("lava-and-rock_albedo.ktx2");
	shader = pDevice->getAPI()->createShader("screenspace");
}

void CRenderSystem::reCreate()
{
	auto createInfo = pParser->reParse();

	for (auto& stage : createInfo.vStages)
	{
		auto& pStage = mStages[stage.srName];
		if (pStage)
			pStage->reCreate(stage);
	}
}

void CRenderSystem::render(vk::CommandBuffer& commandBuffer)
{
	auto pApi = pDevice->getAPI();
	auto& pResources = pApi->getResourceHolder();

	auto& pTexture = pResources->getImage(image);
	auto& pShader = pResources->getShader(shader);

	for (auto& [name, stage] : mStages)
	{
		auto stageFlag = stage->getStageFlag();
		stage->begin(commandBuffer);

		pShader->addTexture("input_tex", pTexture);
		pShader->render(commandBuffer);
		commandBuffer.draw(3, 1, 0, 0);

		switch (stageFlag)
		{
		case engine::ERenderStageAvaliableFlagBits::eShadow:
			renderShadows(commandBuffer); 
			break;
		case engine::ERenderStageAvaliableFlagBits::eMesh:
			renderMeshes(commandBuffer);
			break;
		case engine::ERenderStageAvaliableFlagBits::eComposition:
			renderComposition(commandBuffer);
			break;
		case engine::ERenderStageAvaliableFlagBits::ePostProcess:
			renderPostProcess(commandBuffer);
			break;
		}

		stage->end(commandBuffer);
	}
}

const std::unique_ptr<CRenderStage>& CRenderSystem::getStage(const std::string& srName)
{
	return mStages[srName];
}

void CRenderSystem::setView(const glm::mat4& view)
{
	pView = &view;
}

void CRenderSystem::setProjection(const glm::mat4& projection)
{
	pProjection = &projection;
}

void CRenderSystem::setViewDir(const glm::vec3& viewDir)
{
	pViewDir = &viewDir;
}

void CRenderSystem::setViewportDim(const glm::vec2& viewportDim)
{
	pViewportDim = &viewportDim;
}

void CRenderSystem::setFrustum(const FFrustum& frustum)
{
	pFrustum = &frustum;
}

const FFrustum& CRenderSystem::getFrustum() const
{
	return *pFrustum;
}

void CRenderSystem::renderShadows(vk::CommandBuffer& commandBuffer)
{

}

void CRenderSystem::renderMeshes(vk::CommandBuffer& commandBuffer)
{

}

void CRenderSystem::renderComposition(vk::CommandBuffer& commandBuffer)
{

}

void CRenderSystem::renderPostProcess(vk::CommandBuffer& commandBuffer)
{

}