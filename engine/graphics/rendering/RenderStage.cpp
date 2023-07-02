#include "RenderStage.h"

#include "APIHandle.h"
//#include <glm/gtx/color_cast.hpp>

using namespace engine;
using namespace engine::graphics;

CRenderStage::CRenderStage(CDevice* device)
{
	pDevice = device;
	pFramebuffer = std::make_unique<CFramebuffer>(device);
}

void CRenderStage::create(const FCIStage& createInfo)
{
	pFramebuffer->setRenderArea(createInfo.viewport.offset, createInfo.viewport.extent);
	pFramebuffer->setFlipViewport(createInfo.bFlipViewport);

	for (auto& image : createInfo.vImages)
		pFramebuffer->addImage(image.srName, image.format, image.usage, image.addressMode, image.type, image.layers);

	pFramebuffer->addOutputReference(0, createInfo.vOutputs);

	uint32_t descriptionID{ 0 };
	for (auto& description : createInfo.vDescriptions)
	{
		pFramebuffer->addDescription(descriptionID, description);
		descriptionID++;
	}

	for (auto& dependency : createInfo.vDependencies)
		pFramebuffer->addSubpassDependency
		(
			dependency.src.subpass, 
			dependency.dst.subpass, 
			dependency.src.stageMask, 
			dependency.dst.stageMask, 
			dependency.src.accessMask, 
			dependency.dst.accessMask, 
			dependency.depFlags
		);

	srName = createInfo.srName;

	pFramebuffer->create();
}

void CRenderStage::reCreate(const FCIStage& createInfo)
{
	pFramebuffer->setRenderArea(createInfo.viewport.offset, createInfo.viewport.extent);
	pFramebuffer->reCreate();
}

void CRenderStage::begin(vk::CommandBuffer& commandBuffer)
{
	auto& debugUtils = pDevice->getDebugUtils();

	debugUtils->beginLabel(commandBuffer, srName.c_str());
	pFramebuffer->begin(commandBuffer);
}

void CRenderStage::end(vk::CommandBuffer& commandBuffer)
{
	auto& debugUtils = pDevice->getDebugUtils();

	pFramebuffer->end(commandBuffer);
	debugUtils->endLabel(commandBuffer);
}

const std::unique_ptr<CFramebuffer>& CRenderStage::getFramebuffer() const
{
	return pFramebuffer;
}

CDevice* CRenderStage::getDevice()
{
	return pDevice;
}