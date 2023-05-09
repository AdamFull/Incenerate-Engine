#pragma once

#include <vulkan/vulkan.hpp>

namespace engine
{
	namespace graphics
	{
		class CPipeline
		{
		public:
			CPipeline() = default;
			CPipeline(CDevice* device);
			virtual ~CPipeline();

			virtual void create(CShaderObject* pShader);
			virtual void create(CShaderObject* pShader, vk::RenderPass& renderPass, uint32_t subpass);
			virtual void reCreate(CShaderObject* pShader, vk::RenderPass& renderPass, uint32_t _subpass);

			void bind(vk::CommandBuffer& commandBuffer, uint32_t index = 0);
			const size_t count() const { return vPipelines.size(); }

			vk::Pipeline& getPSO(uint32_t index) { return vPipelines[index]; }
			const std::unordered_map<uint32_t, vk::DescriptorSetLayout>& getDescriptorSetLayouts() { return mDescriptorSetLayouts; }
			vk::DescriptorPool& getDescriptorPool() { return descriptorPool; }
			vk::PipelineLayout& getPipelineLayout() { return pipelineLayout; }

			virtual vk::PipelineBindPoint getBindPoint() { return vk::PipelineBindPoint::eCompute; }

			CDevice* getDevice();

		protected:
			void createDescriptorSetLayout(CShaderObject* pShader);
			void createDescriptorPool(CShaderObject* pShader);
			void createPipelineLayout(CShaderObject* pShader);

			virtual void createPipeline(CShaderObject* pShader) { pShader; }

		protected:
			CDevice* pDevice{ nullptr };

			std::vector<vk::Pipeline> vPipelines;
			std::unordered_map<uint32_t, vk::DescriptorSetLayout> mDescriptorSetLayouts;
			vk::DescriptorPool descriptorPool{ nullptr };
			vk::PipelineLayout pipelineLayout{ nullptr };
			bool bCanBeBindless{ false };
			bool bBindlessFeature{ false };

			vk::RenderPass renderPass{ nullptr };
			uint32_t subpass{ 0 };
		};
	}
}