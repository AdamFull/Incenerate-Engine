#pragma once
#include "RenderStage.h"

namespace engine
{
	namespace graphics
	{
		class CRenderSystem
		{
		public:
			CRenderSystem(CDevice* device);
			void create(const std::string& description);
			void reCreate();
			void render(vk::CommandBuffer& commandBuffer);

			const std::unique_ptr<CRenderStage>& getStage(const std::string& srName);
			void pushShader(CShaderObject* shader);
		private:
			CDevice* pDevice{ nullptr };
			std::map<std::string, std::unique_ptr<CRenderStage>> mStages;
			std::unique_ptr<CRenderSystemParser> pParser;
		};
	}
}