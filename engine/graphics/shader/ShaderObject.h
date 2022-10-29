#pragma once

#include "buffers/VertexBufferObject.h"
#include "handlers/Handler.h"
#include "pipeline/Pipeline.h"
#include "rendering/Framebuffer.h"
#include "descriptors/DescriptorHandler.h"
#include "APIStructures.h"

#include <utility/ufunction.hpp>

namespace engine
{
	namespace graphics
	{
		struct FSOInstance
		{
			std::unique_ptr<CDescriptorHandler> pDescriptorSet;
			std::map<std::string, std::unique_ptr<CHandler>> mBuffers;
			//std::map<std::string, std::unique_ptr<CPushHandler>>
		};

		class CShaderObject
		{
		public:
			friend class CShaderLoader;
			CShaderObject() = default;
			CShaderObject(CDevice* device);

			void create();
			void reCreate();
			void setRenderFunc(utl::function<void(CShaderObject*, vk::CommandBuffer&)>&& rf);
			void render(vk::CommandBuffer& commandBuffer);
			void dispatch(size_t size);

			void bind(vk::CommandBuffer& commandBuffer, bool drawcall = true);

			void addTexture(const std::string& attachment, vk::DescriptorImageInfo& descriptor);
			void addTexture(const std::string& attachment, std::shared_ptr<CImage>& pTexture);
			void addTexture(const std::string& attachment, std::weak_ptr<CImage>& pTexture);

			vk::DescriptorImageInfo& getTexture(const std::string& attachment);

			std::unique_ptr<CHandler>& getUniformBuffer(const std::string& name);
			std::map<std::string, std::unique_ptr<CHandler>>& getUniformBuffers();
			std::unique_ptr<CDescriptorHandler>& getDescriptorSet();

			std::unique_ptr<CPipeline>& getPipeline() { return pPipeline; }
			const std::unique_ptr<CVertexBufferObject>& getVBO() { return pVBO; }
			const std::unique_ptr<CFramebuffer>& getFramebuffer() { return pFramebuffer; }

			vk::PipelineBindPoint getBindPoint() { return programCI.bindPoint; }
			vk::CullModeFlagBits getCullMode() { return programCI.cullMode; }
			vk::FrontFace getFrontFace() { return programCI.frontFace; }
			bool getDepthTestFlag() { return programCI.depthTest; }
			std::vector<vk::DynamicState>& getDynamicStateEnables() { return programCI.dynamicStates; }
			vk::PrimitiveTopology getPrimitiveTopology() { return programCI.topology; }
			bool getTesselationFlag() { return programCI.tesselation; }
			bool isVertexFree() { return programCI.vertexfree; }

			const std::unique_ptr<CShader>& getShader();
		private:
			CDevice* pDevice{ nullptr };

			utl::function<void(CShaderObject*, vk::CommandBuffer&)> pRenderFunc;

			std::unique_ptr<CShader> pShader;
			std::unique_ptr<CPipeline> pPipeline;
			std::unique_ptr<CFramebuffer> pFramebuffer;
			std::unique_ptr<CVertexBufferObject> pVBO;

			uint32_t currentInstance{ 0 }, instances{ 1 };
			bool bIsCreated{ false }, bIsReCreated{ false };

			std::unique_ptr<CHandler> pEmptyHandler{ nullptr };
			std::vector<std::unique_ptr<FSOInstance>> vInstances;
			std::map<std::string, vk::DescriptorImageInfo> mTextures;

			FProgramCreateInfo programCI;
		};
	}
}