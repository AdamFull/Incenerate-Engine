#include "RenderSystem.h"
#include "RenderSystemParser.h"
#include "APIHandle.h"

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
	for (auto& [name, stage] : mStages)
		stage->render(commandBuffer);
}

const std::unique_ptr<CRenderStage>& CRenderSystem::getStage(const std::string& srName)
{
	return mStages[srName];
}

void CRenderSystem::pushShader(CShaderObject* shader)
{
	auto& stage = getStage(shader->getStage());
	if (stage)
		stage->push(shader);
}