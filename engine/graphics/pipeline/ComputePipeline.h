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

			void create(CShaderObject* pShader, const FShaderCreateInfo& specials) override;

		protected:
			void createPipeline(CShaderObject* pShader, const FShaderCreateInfo& specials) override;
			vk::PipelineBindPoint getBindPoint() { return vk::PipelineBindPoint::eCompute; }

		};
	}
}