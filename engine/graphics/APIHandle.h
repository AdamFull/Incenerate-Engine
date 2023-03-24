#pragma once

#include "Device.h"
#include "buffers/CommandBuffer.h"
#include "rendering/QueryPool.h"
#include "EngineStructures.h"

#include "shader/ShaderLoader.h"
#include "graphics/window/WindowAdapter.h"
#include "graphics/image/Image.h"
#include "rendering/RenderStage.h"
#include "rendering/material/Material.h"

#include "DebugDraw.h"

#include "ObjectManager.hpp"

namespace engine
{
	namespace system {
		namespace window {
			class CWindowHandle;
		}
	}

	namespace graphics
	{
		using winhandle_t = IWindowAdapter*;
		class CAPIHandle
		{
		public:
			CAPIHandle(winhandle_t window);
			~CAPIHandle();

			void create(const FEngineCreateInfo& createInfo);
			void update();
			void forceReleaseResources();
			void reCreate(bool bSwapchain, bool bViewport);
			void shutdown();

			const std::unique_ptr<CDebugDraw>& getDebugDraw() const;
			const std::unique_ptr<CQueryPool>& getQueryPool() const;

			vk::CommandBuffer begin();
			void end(float fDT);
			vk::CommandBuffer getCommandBuffer();

			const std::unique_ptr<CDevice>& getDevice() const;
			const std::unique_ptr<CShaderLoader>& getShaderLoader();

			ERenderApi getAPI() { return eAPI; }

			size_t addImage(const std::string& name, std::unique_ptr<CImage>&& image);
			size_t addImage(const std::string& name, const std::filesystem::path& path);
			size_t addImageAsync(const std::string& name, const std::filesystem::path& path);
			void incrementImageUsage(const std::string& name);
			void incrementImageUsage(size_t id);
			void removeImage(const std::string& name);
			void removeImage(size_t id);
			const std::unique_ptr<CImage>& getImage(const std::string& name);
			size_t getImageID(const std::string& name);
			const std::unique_ptr<CImage>& getImage(size_t id);
			void copyImage(vk::CommandBuffer& commandBuffer, size_t src, size_t dst);

			size_t addShader(const std::string& name, std::unique_ptr<CShaderObject>&& shader);
			size_t addShader(const std::string& name, const std::string& shadertype, size_t mat_id = invalid_index);
			size_t addShader(const std::string& name, const std::string& shadertype, const FShaderSpecials& specials);
			void removeShader(const std::string& name);
			void removeShader(size_t id);
			const std::unique_ptr<CShaderObject>& getShader(const std::string& name);
			const std::unique_ptr<CShaderObject>& getShader(size_t id);

			size_t addMaterial(const std::string& name, std::unique_ptr<CMaterial>&& material);
			void removeMaterial(const std::string& name);
			void removeMaterial(size_t id);
			const std::unique_ptr<CMaterial>& getMaterial(const std::string& name);
			const std::unique_ptr<CMaterial>& getMaterial(size_t id);

			size_t addVertexBuffer(const std::string& name);
			size_t addVertexBuffer(const std::string& name, std::unique_ptr<CVertexBufferObject>&& vbo);
			void removeVertexBuffer(const std::string& name);
			void removeVertexBuffer(size_t id);
			const std::unique_ptr<CVertexBufferObject>& getVertexBuffer(const std::string& name);
			const std::unique_ptr<CVertexBufferObject>& getVertexBuffer(size_t id);

			size_t addRenderStage(const std::string& name);
			size_t addRenderStage(const std::string& name, std::unique_ptr<CRenderStage>&& stage);
			void removeRenderStage(const std::string& name);
			void removeRenderStage(size_t id);
			const std::unique_ptr<CRenderStage>& getRenderStage(const std::string& name);
			const std::unique_ptr<CRenderStage>& getRenderStage(size_t id);
			size_t getRenderStageID(const std::string& name);

			const std::unique_ptr<CFramebuffer>& getFramebuffer(const std::string& srName);

			size_t computeBRDFLUT(uint32_t size);
			size_t computeIrradiance(size_t origin, uint32_t size);
			size_t computePrefiltered(size_t origin, uint32_t size);

			// Drawing methods
			void bindShader(size_t id);
			void bindMaterial(size_t id);
			bool bindVertexBuffer(size_t id);
			void bindRenderer(size_t id);
			void bindTexture(const std::string& name, size_t id, uint32_t mip_level = 0);

			void clearQuery();
			void beginQuery(uint32_t index);
			void endQuery(uint32_t index);
			void getQueryResult();
			bool occlusionTest(uint32_t index);

			bool compareAlphaMode(EAlphaMode mode);

			void setManualShaderControlFlag(bool value) { bManualShaderControl = value; }
			void flushConstantRanges(const std::unique_ptr<CPushHandler>& constantRange);
			void flushShader();

			const std::unique_ptr<CHandler>& getUniformHandle(const std::string& name);
			const std::unique_ptr<CPushHandler>& getPushBlockHandle(const std::string& name);

			void draw(size_t begin_vertex = 0, size_t vertex_count = 0, size_t begin_index = 0, size_t index_count = 0, size_t instance_count = 1);
			void dispatch(const std::vector<FDispatchParam>& params);
			void dispatch(const FDispatchParam& param);

		private:
			vk::CommandBuffer beginFrame();
			vk::Result endFrame();

		protected:
			winhandle_t pWindow{ nullptr };

			ERenderApi eAPI;
			std::unique_ptr<CDevice> pDevice;
			std::unique_ptr<CShaderLoader> pLoader;
			std::unique_ptr<CDebugDraw> pDebugDraw;
			std::unique_ptr<CQueryPool> pQueryPool;

			std::unordered_map<std::string, FCIStage> mStageInfos;
			std::unique_ptr<CObjectManager<CImage>> pImageManager;
			std::unique_ptr<CObjectManager<CShaderObject>> pShaderManager;
			std::unique_ptr<CObjectManager<CMaterial>> pMaterialManager;
			std::unique_ptr<CObjectManager<CVertexBufferObject>> pVertexBufferManager;
			std::unique_ptr<CObjectManager<CRenderStage>> pRenderStageManager;

			CShaderObject* pBindedShader{ nullptr };
			CMaterial* pBindedMaterial{ nullptr };
			CVertexBufferObject* pBindedVBO{ nullptr };
			CRenderStage* pBindedRenderStage{ nullptr };
			bool bManualShaderControl{ false };

			uint32_t imageIndex{ 0 };
			bool frameStarted{ false };
			std::unique_ptr<CCommandBuffer> commandBuffers;
		};
	}
}