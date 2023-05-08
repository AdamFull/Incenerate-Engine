#pragma once

#include <vulkan/vulkan.hpp>

namespace engine
{
	namespace graphics
	{
		class CDescriptorSet : public utl::non_copyable
		{
		public:
			CDescriptorSet(CDevice* device);
			~CDescriptorSet();

			void create(const vk::DescriptorPool& pool, const vk::DescriptorSetLayout& descriptorSetLayout);
			void update(std::vector<vk::WriteDescriptorSet>& vWrites);
			void update(vk::WriteDescriptorSet& writes);
			void bind(const vk::CommandBuffer& commandBuffer, const vk::PipelineBindPoint pipelineBindPoint, const vk::PipelineLayout& pipelineLayout) const;

			vk::DescriptorSet& get();

			CDevice* getDevice();

		private:
			CDevice* pDevice{ nullptr };
			std::vector<vk::DescriptorSet> vDescriptorSets;
			vk::DescriptorPool descriptorPool{ nullptr };
		};
	}
}