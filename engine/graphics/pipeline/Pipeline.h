#pragma once

#include <vulkan/vulkan.hpp>

namespace engine
{
	namespace graphics
	{
		class CDevice;
		class CShaderObject;

		class CPipeline
		{
		public:
			CPipeline();
			CPipeline(CDevice* device);
			virtual ~CPipeline();

			virtual void create(CShaderObject* pShader);
			virtual void create(CShaderObject* pShader, vk::RenderPass& renderPass, uint32_t subpass);
			virtual void reCreate(CShaderObject* pShader, vk::RenderPass& renderPass, uint32_t _subpass);

			void bind(vk::CommandBuffer& commandBuffer);

			virtual vk::PipelineBindPoint getBindPoint() { return vk::PipelineBindPoint::eCompute; }

		protected:
			void createDescriptorSetLayout(CShaderObject* pShader);
			void createDescriptorPool(CShaderObject* pShader);
			void createPipelineLayout(CShaderObject* pShader);

			virtual void createPipeline(CShaderObject* pShader) {}

		protected:
			CDevice* pDevice{ nullptr };

			vk::Pipeline pipeline{ nullptr };
			vk::DescriptorSetLayout descriptorSetLayout{ nullptr };
			vk::DescriptorPool descriptorPool{ nullptr };
			vk::PipelineLayout pipelineLayout{ nullptr };

			vk::RenderPass renderPass{ nullptr };
			uint32_t subpass{ 0 };
		};
	}
}