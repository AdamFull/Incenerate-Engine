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
	
	for (auto& [set, descriptorSetLayout] : mDescriptorSetLayouts)
	{
		auto pDescriptorSet = std::make_unique<CDescriptorSet>(pDevice);
		pDescriptorSet->create(descriptorPool, descriptorSetLayout);
		mDescriptorSets.emplace(set, std::move(pDescriptorSet));
	}
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

	std::vector<vk::DescriptorSet> vDescriptorSets;
	for (auto& [set, descriptorSet] : mDescriptorSets)
		vDescriptorSets.emplace_back(descriptorSet->get());

	commandBuffer.bindDescriptorSets(bindPoint, pipelineLayout, 0, vDescriptorSets.size(), vDescriptorSets.data(), 0, nullptr);
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

	if (mDescriptorSets.count(uniformBlock->getSet()) == 0)
		return;
	
	vk::WriteDescriptorSet write{};
	write.descriptorType = uniformBlock->getDescriptorType();
	write.dstBinding = uniformBlock->getBinding();
	write.dstSet = mDescriptorSets[uniformBlock->getSet()]->get();
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

	if (mDescriptorSets.count(uniform->getSet()) == 0)
		return;
	
	vk::WriteDescriptorSet write{};
	write.descriptorType = uniform->getDescriptorType();
	write.dstBinding = uniform->getBinding();
	write.dstSet = mDescriptorSets[uniform->getSet()]->get();
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

	if (mDescriptorSets.count(uniformBlock->getSet()) == 0)
		return;

	writeInfo.dstSet = mDescriptorSets[uniformBlock->getSet()]->get();

	vWriteDescriptorSets.emplace_back(writeInfo);
}

CDevice* CDescriptorHandler::getDevice()
{
	return pDevice;
}