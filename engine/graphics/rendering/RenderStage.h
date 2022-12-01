#pragma once
#include "Framebuffer.h"
#include "RenderSystemParser.h"

namespace engine
{
	namespace graphics
	{
		class CRenderStage
		{
		public:
			CRenderStage(CDevice* device);
			void create(const FCIStage& createInfo);
			void reCreate(const FCIStage& createInfo);
			void render(vk::CommandBuffer& commandBuffer);

			void push(CShaderObject* shader);

			const std::unique_ptr<CFramebuffer>& getFramebuffer() const;
		private:
			CDevice* pDevice{ nullptr };
			FCIStage stageCI;
			std::unique_ptr<CFramebuffer> pFramebuffer;
			std::vector<CShaderObject*> vRenderQueue;
		};
	}
}