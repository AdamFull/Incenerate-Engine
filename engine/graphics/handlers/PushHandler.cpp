#include "PushHandler.h"

#include "Engine.h"

using namespace engine::graphics;

CPushHandler::CPushHandler(CPipeline* pipe)
{
	pPipeline = pipe;
}

CPushHandler::~CPushHandler()
{
	vData.clear();
}

void CPushHandler::create(const CPushConstBlock& pushBlock)
{
	auto& graphics = EGEngine->getGraphics();

	auto images = graphics->getDevice()->getFramesInFlight();

	for(auto i = 0; i < images; i++)
		vData.emplace_back(std::make_unique<char[]>(pushBlock.getSize()));
	this->pushBlock = pushBlock;
}

void CPushHandler::flush(vk::CommandBuffer& commandBuffer)
{
	auto index = getCurrentFrameProxy();

	auto& data = vData.at(index);
	if (data && pPipeline)
		commandBuffer.pushConstants(pPipeline->getPipelineLayout(), pushBlock->getStageFlags(), 0, static_cast<uint32_t>(pushBlock->getSize()), data.get());
}

uint32_t CPushHandler::getCurrentFrameProxy()
{
	auto& graphics = EGEngine->getGraphics();
	auto& device = graphics->getDevice();
	return device->getCurrentFrame();
}