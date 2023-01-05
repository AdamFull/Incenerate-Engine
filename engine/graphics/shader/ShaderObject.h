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
		class CShaderObject
		{
		public:
			friend class CShaderLoader;
			CShaderObject() = default;
			CShaderObject(CDevice* device);
			~CShaderObject();

			void create();
			void predraw(vk::CommandBuffer& commandBuffer);
			void dispatch(size_t size);

			void addTexture(const std::string& attachment, size_t id);

			vk::DescriptorImageInfo& getTexture(const std::string& attachment);

			std::unique_ptr<CHandler>& getUniformBuffer(const std::string& name);

			std::unique_ptr<CPipeline>& getPipeline() { return pPipeline; }

			vk::PipelineBindPoint getBindPoint() const { return programCI.bindPoint; }
			vk::CullModeFlagBits getCullMode() const { return programCI.cullMode; }
			vk::FrontFace getFrontFace() const { return programCI.frontFace; }
			bool getDepthTestFlag() const { return programCI.depthTest; }
			const std::vector<vk::DynamicState>& getDynamicStateEnables() const { return programCI.dynamicStates; }
			vk::PrimitiveTopology getPrimitiveTopology() const { return programCI.topology; }
			bool getTesselationFlag() const { return programCI.tesselation; }
			bool isVertexFree() const { return programCI.vertexfree; }
			const std::string& getStage() const { return programCI.srStage; }

			const std::unique_ptr<CShader>& getShader();
		private:
			CDevice* pDevice{ nullptr };
			uint32_t iVBOid{ 0 };

			std::unique_ptr<CShader> pShader;
			std::unique_ptr<CPipeline> pPipeline;
			std::map<std::string, vk::DescriptorImageInfo> mTextures;
			std::unique_ptr<CDescriptorHandler> pDescriptorSet;
			std::map<std::string, std::unique_ptr<CHandler>> mBuffers;

			std::unique_ptr<CHandler> pEmptyHandler{ nullptr };

			FProgramCreateInfo programCI;
		};
	}
}