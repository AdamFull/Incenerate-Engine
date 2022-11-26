#pragma once

#include "Device.h"
#include "buffers/CommandBuffer.h"
#include "shader/ShaderLoader.h"
#include "buffers/VertexBufferObject.h"
#include "EngineEnums.h"
#include "graphics/image/Image.h"
#include "graphics/rendering/RenderSystem.h"

namespace engine
{
	namespace system {
		namespace window {
			class CWindowHandle;
		}
	}

	namespace graphics
	{
		using winhandle_t = system::window::CWindowHandle*;
		class CAPIHandle
		{
		public:
			CAPIHandle(winhandle_t window);

			void create(const FEngineCreateInfo& createInfo);
			void reCreate();
			void shutdown();
			void render();

			const std::unique_ptr<CDevice>& getDevice() const;
			const std::unique_ptr<CShaderLoader>& getShaderLoader();
			const std::unique_ptr<CRenderSystem>& getRenderSystem();
			std::unique_ptr<CVertexBufferObject> allocateVBO();

			ERenderApi getAPI() { return eAPI; }

			const std::unique_ptr<CFramebuffer>& getFramebuffer(const std::string& srName);

		private:
			vk::CommandBuffer beginFrame();
			vk::Result endFrame();

		protected:
			winhandle_t pWindow{ nullptr };

			ERenderApi eAPI;
			std::unique_ptr<CDevice> pDevice;
			std::unique_ptr<CShaderLoader> pLoader;
			std::unique_ptr<CRenderSystem> pRenderSystem;

			uint32_t imageIndex{ 0 };
			bool frameStarted{ false };
			vk::Extent2D screenExtent{};
			std::unique_ptr<CCommandBuffer> commandBuffers;
		};
	}
}