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

			void create(CShaderObject* pShader, vk::RenderPass& renderPass, const FShaderCreateInfo& specials) override;

		protected:
			void createPipeline(CShaderObject* pShader, const FShaderCreateInfo& specials) override;
			vk::PipelineBindPoint getBindPoint() { return vk::PipelineBindPoint::eRayTracingKHR; }
		};
	}
}