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

			void create(CShaderObject* pShader, vk::RenderPass& renderPass, uint32_t subpass) override;

		protected:
			void createPipeline(CShaderObject* pShader) override;
			vk::PipelineBindPoint getBindPoint() { return vk::PipelineBindPoint::eGraphics; }

		};
	}
}