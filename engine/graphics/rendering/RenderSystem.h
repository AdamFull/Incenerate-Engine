#pragma once
#include "RenderStage.h"
#include "graphics/Frustum.h"

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

			void setView(const glm::mat4& view);
			void setProjection(const glm::mat4& projection);
			void setViewDir(const glm::vec3& viewDir);
			void setViewportDim(const glm::vec2& viewportDim);
			void setFrustum(const FFrustum& frustum);

			const FFrustum& getFrustum() const;
		private:
			void renderShadows(vk::CommandBuffer& commandBuffer);
			void renderMeshes(vk::CommandBuffer& commandBuffer);
			void renderComposition(vk::CommandBuffer& commandBuffer);
			void renderPostProcess(vk::CommandBuffer& commandBuffer);
		private:
			CDevice* pDevice{ nullptr };
			std::map<std::string, std::unique_ptr<CRenderStage>> mStages;
			std::unique_ptr<CRenderSystemParser> pParser;

			const glm::mat4* pView{ nullptr };
			const glm::mat4* pProjection{ nullptr };
			const glm::vec3* pViewDir{ nullptr };
			const glm::vec2* pViewportDim{ nullptr };
			const FFrustum* pFrustum{ nullptr };
		};
	}
}