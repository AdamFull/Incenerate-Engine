#include "RenderStage.h"

#include "APIHandle.h"

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
		pFramebuffer->addImage(image.srName, image.format, image.usage, image.type, image.layers);

	uint32_t outputID{ 0 };
	for (auto& output : createInfo.vOutputs)
	{
		pFramebuffer->addOutputReference(outputID, output);
		outputID++;
	}

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

	pFramebuffer->create();

	pImage = std::make_unique<CImage>(pDevice);
	pImage->create("lava-and-rock_albedo.ktx2");

	pShader = pDevice->getAPI()->getShaderLoader()->load("screenspace");
	pShader->addTexture("input_tex", pImage);
}

void CRenderStage::reCreate(const FCIStage& createInfo)
{
	auto renderArea = pFramebuffer->getRenderArea();
	if (renderArea.extent != createInfo.viewport.extent)
	{
		pFramebuffer->setRenderArea(createInfo.viewport.offset, createInfo.viewport.extent);
		pFramebuffer->reCreate();
	}
}

void CRenderStage::render(vk::CommandBuffer& commandBuffer)
{
	pFramebuffer->begin(commandBuffer);
	//Render here, but how?
	pShader->render(commandBuffer);
	pFramebuffer->end(commandBuffer);
}

const std::unique_ptr<CFramebuffer>& CRenderStage::getFramebuffer() const
{
	return pFramebuffer;
}