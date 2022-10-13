#pragma once

#include <vulkan/vulkan.hpp>

#include "shader/ShaderObject.h"

namespace engine
{
	namespace graphics
	{
		class CDevice;

		class CPipeline
		{
		public:
			CPipeline(CDevice* device);
			virtual ~CPipeline();

			virtual void create(const std::unique_ptr<CShaderObject>& pShader);
			virtual void create(const std::unique_ptr<CShaderObject>& pShader, vk::RenderPass& renderPass, uint32_t subpass);
			virtual void reCreate(const std::unique_ptr<CShaderObject>& pShader, vk::RenderPass& renderPass, uint32_t _subpass);

			void bind(vk::CommandBuffer& commandBuffer);

			virtual vk::PipelineBindPoint getBindPoint() { return vk::PipelineBindPoint::eCompute; }

		protected:
			void createDescriptorSetLayout(const std::unique_ptr<CShaderObject>& pShader);
			void createDescriptorPool(const std::unique_ptr<CShaderObject>& pShader);
			void createPipelineLayout(const std::unique_ptr<CShaderObject>& pShader);

			virtual void createPipeline(const std::unique_ptr<CShaderObject>& pShader) {}

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