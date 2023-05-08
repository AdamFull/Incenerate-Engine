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

			void create(const vk::DescriptorPool& pool, const std::unordered_map<uint32_t, vk::DescriptorSetLayout>& descriptorSetLayouts);
			void update(std::vector<vk::WriteDescriptorSet>& vWrites);
			void update(vk::WriteDescriptorSet& writes);
			void bind(const vk::CommandBuffer& commandBuffer, const vk::PipelineBindPoint pipelineBindPoint, const vk::PipelineLayout& pipelineLayout) const;

			vk::DescriptorSet& get(uint32_t set = 0);

			CDevice* getDevice();

		private:
			CDevice* pDevice{ nullptr };
			std::unordered_map<uint32_t, std::vector<vk::DescriptorSet>> mDescriptorSets;
			vk::DescriptorPool descriptorPool{ nullptr };
		};
	}
}