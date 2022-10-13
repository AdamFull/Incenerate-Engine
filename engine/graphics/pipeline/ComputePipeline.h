#pragma once

#include "Pipeline.h"

namespace engine
{
	namespace graphics
	{
		class CComputePipeline : public CPipeline
		{
		public:
			CComputePipeline(CDevice* device);
			virtual ~CComputePipeline() = default;

			void create(const std::unique_ptr<CShaderObject>& pShader, vk::RenderPass& renderPass, uint32_t subpass) override;

		protected:
			void createPipeline(const std::unique_ptr<CShaderObject>& pShader) override;
			vk::PipelineBindPoint getBindPoint() { return vk::PipelineBindPoint::eGraphics; }

		};
	}
}