#pragma once

#include "Device.h"
#include "buffers/CommandBuffer.h"
#include "shader/ShaderLoader.h"
#include "EngineStructures.h"
#include "graphics/image/Image.h"
#include "ResourceHolder.h"
#include "rendering/RenderStage.h"

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

			vk::CommandBuffer begin();
			void end();

			const std::unique_ptr<CDevice>& getDevice() const;
			const std::unique_ptr<CShaderLoader>& getShaderLoader();
			const std::unique_ptr<CResourceHolder>& getResourceHolder();
			std::unique_ptr<CVertexBufferObject> allocateVBO();

			ERenderApi getAPI() { return eAPI; }

			const std::unique_ptr<CRenderStage>& getRenderStage(const std::string& srName);
			const std::unique_ptr<CRenderStage>& getRenderStage(size_t id);
			const std::unique_ptr<CFramebuffer>& getFramebuffer(const std::string& srName);

			const std::unique_ptr<CShaderObject>& getShader(size_t id);
			const std::unique_ptr<CImage>& getImage(size_t id);
			const std::unique_ptr<CMaterial>& getMaterial(size_t id);
			const std::unique_ptr<CVertexBufferObject>& getVertexBuffer(size_t id);

			size_t createRenderStage(const std::string& srName);
			size_t createShader(const std::string& srName);
			size_t createImage(const std::string& srPath);
			size_t createMaterial(const std::string& srName);
			size_t createVBO();

		private:
			vk::CommandBuffer beginFrame();
			vk::Result endFrame();

		protected:
			winhandle_t pWindow{ nullptr };

			ERenderApi eAPI;
			std::unique_ptr<CDevice> pDevice;
			std::unique_ptr<CShaderLoader> pLoader;
			std::unique_ptr<CResourceHolder> pResourceHolder;
			std::unordered_map<std::string, size_t> mStages;

			uint32_t imageIndex{ 0 };
			bool frameStarted{ false };
			vk::Extent2D screenExtent{};
			std::unique_ptr<CCommandBuffer> commandBuffers;
		};
	}
}