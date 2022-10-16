#pragma once

#include "Pipeline.h"

namespace engine
{
	namespace graphics
	{
		class CRayTracePipeline : public CPipeline
		{
		public:
			CRayTracePipeline(CDevice* device);
			virtual ~CRayTracePipeline() = default;

			void create(CShaderObject* pShader, vk::RenderPass& renderPass, uint32_t subpass) override;

		protected:
			void createPipeline(CShaderObject* pShader) override;
			vk::PipelineBindPoint getBindPoint() { return vk::PipelineBindPoint::eRayTracingKHR; }
		};
	}
}