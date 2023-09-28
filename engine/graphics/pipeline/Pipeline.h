#pragma once

#include "APIStructures.h"

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

			virtual void create(CShaderObject* pShader, const FShaderCreateInfo& specials);
			virtual void create(CShaderObject* pShader, vk::RenderPass& renderPass, const FShaderCreateInfo& specials);
			virtual void reCreate(CShaderObject* pShader, vk::RenderPass& renderPass, const FShaderCreateInfo& specials);

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
			void createDescriptorPool(CShaderObject* pShader, const FShaderCreateInfo& specials);
			void createPipelineLayout(CShaderObject* pShader, const FShaderCreateInfo& specials);

			virtual void createPipeline(CShaderObject* pShader, const FShaderCreateInfo& specials) { pShader; }

		protected:
			CDevice* pDevice{ nullptr };

			std::vector<vk::Pipeline> vPipelines;
			std::unordered_map<uint32_t, vk::DescriptorSetLayout> mDescriptorSetLayouts;
			vk::DescriptorPool descriptorPool{ nullptr };
			vk::PipelineLayout pipelineLayout{ nullptr };

			vk::RenderPass renderPass{ nullptr };
			uint32_t subpass{ 0 };
		};
	}
}