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

			void create(CShaderObject* pShader) override;

		protected:
			void createPipeline(CShaderObject* pShader) override;
			vk::PipelineBindPoint getBindPoint() { return vk::PipelineBindPoint::eCompute; }

		};
	}
}