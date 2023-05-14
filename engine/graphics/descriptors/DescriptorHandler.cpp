#include "DescriptorHandler.h"

#include "APIHandle.h"
#include "shader/ShaderObject.h"

using namespace engine::graphics;

CDescriptorHandler::CDescriptorHandler(CDevice* device, CShader* shader) :
	pDevice{device}, pShader{shader}
{
}

void CDescriptorHandler::create(const vk::DescriptorPool& descriptorPool, const std::unordered_map<uint32_t, vk::DescriptorSetLayout>& descriptorSetLayouts)
{
	pDescriptorSet = std::make_unique<CDescriptorSet>(pDevice);
	pDescriptorSet->create(descriptorPool, descriptorSetLayouts);
}

void CDescriptorHandler::update()
{
	auto& vkDevice = pDevice->getLogical();
	log_cerror(vkDevice, "Trying to update descriptor sets, but device is invalid.");
	vkDevice.updateDescriptorSets(static_cast<uint32_t>(vWriteDescriptorSets.size()), vWriteDescriptorSets.data(), 0, nullptr);
}

void CDescriptorHandler::bind(const vk::CommandBuffer& commandBuffer, vk::PipelineBindPoint bindPoint, const vk::PipelineLayout& pipelineLayout) const
{
	auto graphics = pDevice->getAPI();
	pDescriptorSet->bind(commandBuffer, bindPoint, pipelineLayout);
}

void CDescriptorHandler::reset()
{
	vWriteDescriptorSets.clear();
}

void CDescriptorHandler::set(const std::string& srUniformName, vk::DescriptorBufferInfo& bufferInfo)
{
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