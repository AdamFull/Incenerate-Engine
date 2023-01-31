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

			void create(vk::PipelineBindPoint bindPoint, vk::PipelineLayout& layout, vk::DescriptorPool& pool, vk::DescriptorSetLayout& setLayout);
			void update(std::vector<vk::WriteDescriptorSet>& vWrites);
			void update(vk::WriteDescriptorSet& writes);
			void bind(const vk::CommandBuffer& commandBuffer) const;

			vk::DescriptorSet& get();

		private:
			CDevice* pDevice{ nullptr };
			std::vector<vk::DescriptorSet> vDescriptorSets;
			vk::PipelineBindPoint pipelineBindPoint;
			vk::PipelineLayout pipelineLayout{ nullptr };
			vk::DescriptorPool descriptorPool{ nullptr };
		};
	}
}