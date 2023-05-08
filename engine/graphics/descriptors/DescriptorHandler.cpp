#include "DescriptorHandler.h"

#include "Device.h"
#include "shader/ShaderObject.h"

using namespace engine::graphics;

CDescriptorHandler::CDescriptorHandler(CDevice* device)
{
	pDevice = device;
}

void CDescriptorHandler::create(CShaderObject* pso)
{	
	pShaderObject = pso;

	auto& pipeline = pShaderObject->getPipeline();
	auto& descriptorPool = pipeline->getDescriptorPool();
	auto& mDescriptorSetLayouts = pipeline->getDescriptorSetLayouts();
	
	pDescriptorSet = std::make_unique<CDescriptorSet>(pDevice);
	pDescriptorSet->create(descriptorPool, mDescriptorSetLayouts);
}

void CDescriptorHandler::update()
{
	auto& vkDevice = pDevice->getLogical();
	log_cerror(vkDevice, "Trying to update descriptor sets, but device is invalid.");
	vkDevice.updateDescriptorSets(static_cast<uint32_t>(vWriteDescriptorSets.size()), vWriteDescriptorSets.data(), 0, nullptr);
}

void CDescriptorHandler::bind(const vk::CommandBuffer& commandBuffer) const
{
	auto bindPoint = pShaderObject->getBindPoint();
	auto& pipeline = pShaderObject->getPipeline();
	auto& pipelineLayout = pipeline->getPipelineLayout();

	pDescriptorSet->bind(commandBuffer, bindPoint, pipelineLayout);
}

void CDescriptorHandler::reset()
{
	vWriteDescriptorSets.clear();
}

void CDescriptorHandler::set(const std::string& srUniformName, vk::DescriptorBufferInfo& bufferInfo)
{
	auto& pShader = pShaderObject->getShader();

	auto uniformBlock = pShader->getUniformBlock(srUniformName);
	if (!uniformBlock)
		return;
	
	vk::WriteDescriptorSet write{};
	write.descriptorType = uniformBlock->getDescriptorType();
	write.dstBinding = uniformBlock->getBinding();
	write.dstSet = pDescriptorSet->get(uniformBlock->getSet());
	write.pBufferInfo = &bufferInfo;
	write.descriptorCount = 1;
	vWriteDescriptorSets.emplace_back(std::move(write));
}

void CDescriptorHandler::set(const std::string& srUniformName, vk::DescriptorImageInfo& imageInfo)
{
	auto& pShader = pShaderObject->getShader();

	auto uniform = pShader->getUniform(srUniformName);
	if (!uniform)
		return;
	
	vk::WriteDescriptorSet write{};
	write.descriptorType = uniform->getDescriptorType();
	write.dstBinding = uniform->getBinding();
	write.dstSet = pDescriptorSet->get(uniform->getSet());
	write.pImageInfo = &imageInfo;
	write.descriptorCount = 1;
	vWriteDescriptorSets.emplace_back(std::move(write));
}

void CDescriptorHandler::set(const std::string& srUniformName, vk::WriteDescriptorSet& writeInfo)
{
	auto& pShader = pShaderObject->getShader();

	auto uniformBlock = pShader->getUniformBlock(srUniformName);
	if (!uniformBlock)
		return;

	writeInfo.dstSet = pDescriptorSet->get(uniformBlock->getSet());
	vWriteDescriptorSets.emplace_back(writeInfo);
}

CDevice* CDescriptorHandler::getDevice()
{
	return pDevice;
}