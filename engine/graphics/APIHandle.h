#pragma once

#include "Device.h"
#include "buffers/CommandBuffer.h"
#include "shader/ShaderLoader.h"
#include "buffers/VertexBufferObject.h"
#include "EngineEnums.h"

namespace engine
{
	namespace graphics
	{
		class CAPIHandle
		{
		public:
			void create(const FEngineCreateInfo& createInfo);
			void reCreate();
			void render();

			const std::unique_ptr<CShaderObject>& addStage(const std::string& shader_name);

			const std::unique_ptr<CDevice>& getDevice() const;
			const std::unique_ptr<CShaderLoader>& getShaderLoader();
			std::unique_ptr<CVertexBufferObject> allocateVBO();

			ERenderApi getAPI() { return eAPI; }

		private:
			vk::CommandBuffer beginFrame();
			vk::Result endFrame();

		protected:
			ERenderApi eAPI;
			std::unique_ptr<CDevice> pDevice;
			std::unique_ptr<CShaderLoader> pLoader;

			std::vector<std::unique_ptr<CShaderObject>> vStages;

			uint32_t imageIndex{ 0 };
			bool frameStarted{ false };
			vk::Extent2D screenExtent{};
			std::unique_ptr<CCommandBuffer> commandBuffers;
		};
	}
}