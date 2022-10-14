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
	auto& pipeline = pso->getPipeline();
	auto bindPoint = pso->getBindPoint();
	auto& layout = pipeline->getPipelineLayout();
	auto& descriptorPool = pipeline->getDescriptorPool();
	auto& descriptorSetLayout = pipeline->getDescriptorSetLayout();

	pShader = pso->getShader().get();
	
	pDescriptorSet = utl::make_scope<CDescriptorSet>(pDevice);
	pDescriptorSet->create(bindPoint, layout, descriptorPool, descriptorSetLayout);
}

void CDescriptorHandler::update()
{
	auto& vkDevice = pDevice->getLogical();
	assert(vkDevice && "Trying to update descriptor sets, but device is invalid.");
	for (auto& write : vWriteDescriptorSets)
		write.dstSet = pDescriptorSet->get();
	vkDevice.updateDescriptorSets(static_cast<uint32_t>(vWriteDescriptorSets.size()), vWriteDescriptorSets.data(), 0, nullptr);
}

void CDescriptorHandler::bind(const vk::CommandBuffer& commandBuffer) const
{
	pDescriptorSet->bind(commandBuffer);
}

void CDescriptorHandler::reset()
{
	vWriteDescriptorSets.clear();
}

void CDescriptorHandler::set(const std::string& srUniformName, vk::DescriptorBufferInfo& bufferInfo)
{
	auto uniformBlock = pShader->getUniformBlock(srUniformName);
	
	vk::WriteDescriptorSet write{};
	write.descriptorType = uniformBlock->getDescriptorType();
	write.dstBinding = uniformBlock->getBinding();
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
	write.pImageInfo = &imageInfo;
	write.descriptorCount = 1;
	vWriteDescriptorSets.emplace_back(std::move(write));
}

void CDescriptorHandler::set(const std::string& srUniformName, vk::WriteDescriptorSet& writeInfo)
{
	vWriteDescriptorSets.emplace_back(std::move(writeInfo));
}