#pragma once

#include "DescriptorSet.h"

namespace engine
{
	namespace graphics
	{
		class CDescriptorHandler
		{
		public:
			CDescriptorHandler(CDevice* device, CShader* shader);

			void create(const vk::DescriptorPool& descriptorPool, const std::unordered_map<uint32_t, vk::DescriptorSetLayout>& descriptorSetLayouts);
			void update();
			void bind(const vk::CommandBuffer& commandBuffer, vk::PipelineBindPoint bindPoint, const vk::PipelineLayout& pipelineLayout) const;
			void reset();

			void set(const std::string& srUniformName, vk::DescriptorBufferInfo& bufferInfo);
			void set(const std::string& srUniformName, vk::DescriptorImageInfo& imageInfo);
			void set(const std::string& srUniformName, vk::WriteDescriptorSet& writeInfo);

			CDevice* getDevice();
		private:
			CDevice* pDevice{ nullptr };
			CShader* pShader{ nullptr };
			uint32_t updatedInFrame{ -1u };
			std::unique_ptr<CDescriptorSet> pDescriptorSet;
			std::vector<vk::WriteDescriptorSet> vWriteDescriptorSets;
		};
	}
}