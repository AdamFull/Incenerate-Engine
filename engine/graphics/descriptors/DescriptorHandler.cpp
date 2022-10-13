#include "DescriptorHandler.h"


using namespace engine::graphics;

CDescriptorHandler::CDescriptorHandler(CDevice* device)
{
	pDevice = device;
}

void CDescriptorHandler::create(CShaderObject* pipeline)
{
	//pShader = pipeline->getShader().get();
	//
	//auto bindPoint = pipeline->get<vk::PipelineBindPoint>();
	//auto layout = pipeline->get<vk::PipelineLayout>();
	//auto descriptorPool = pipeline->get<vk::DescriptorPool>();
	//auto descriptorSetLayout = pipeline->get<vk::DescriptorSetLayout>();
	//
	//pDescriptorSet = utl::make_scope<CDescriptorSet>(pDevice);
	//pDescriptorSet->create(bindPoint, layout, descriptorPool, descriptorSetLayout);
}

void CDescriptorHandler::update()
{
	//auto& vkDevice = pDevice->getLogical();
	//assert(vkDevice && "Trying to update descriptor sets, but device is invalid.");
	//for (auto& write : vWriteDescriptorSets)
	//	write.dstSet = pDescriptorSet->get();
	//vkDevice.updateDescriptorSets(static_cast<uint32_t>(vWriteDescriptorSets.size()), vWriteDescriptorSets.data(), 0, nullptr);
}

void CDescriptorHandler::bind(const vk::CommandBuffer& commandBuffer) const
{
	//pDescriptorSet->bind(commandBuffer);
}

void CDescriptorHandler::reset()
{
	//vWriteDescriptorSets.clear();
}

void CDescriptorHandler::set(const std::string& srUniformName, vk::DescriptorBufferInfo& bufferInfo)
{
	//auto uniformBlock = pShader->getUniformBlock(srUniformName);
	//auto descriptorType = uniformBlock->getDescriptorType();
	//
	//vk::WriteDescriptorSet write{};
	//write.descriptorType = VulkanStaticHelper::getDescriptorType(descriptorType);
	//write.dstBinding = uniformBlock->getBinding();
	//write.pBufferInfo = &bufferInfo;
	//write.descriptorCount = 1;
	//vWriteDescriptorSets.emplace_back(std::move(write));
}

void CDescriptorHandler::set(const std::string& srUniformName, vk::DescriptorImageInfo& imageInfo)
{
	//auto uniform = pShader->getUniform(srUniformName);
	//if (!uniform)
	//	return;
	//auto descriptorType = uniform->getDescriptorType();
	//
	//vk::WriteDescriptorSet write{};
	//write.descriptorType = VulkanStaticHelper::getDescriptorType(descriptorType);
	//write.dstBinding = uniform->getBinding();
	//write.pImageInfo = &imageInfo;
	//write.descriptorCount = 1;
	//vWriteDescriptorSets.emplace_back(std::move(write));
}

void CDescriptorHandler::set(const std::string& srUniformName, vk::WriteDescriptorSet& writeInfo)
{
	//vWriteDescriptorSets.emplace_back(std::move(writeInfo));
}