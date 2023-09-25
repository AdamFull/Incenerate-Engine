#pragma once

#include "Pipeline.h"

namespace engine
{
	namespace graphics
	{
		class CGraphicsPipeline : public CPipeline
		{
		public:
			CGraphicsPipeline(CDevice* device);
			virtual ~CGraphicsPipeline() = default;

			void create(CShaderObject* pShader, vk::RenderPass& renderPass, const FShaderCreateInfo& specials) override;

		protected:
			void createPipeline(CShaderObject* pShader, const FShaderCreateInfo& specials) override;
			vk::PipelineBindPoint getBindPoint() { return vk::PipelineBindPoint::eGraphics; }

		};
	}
}