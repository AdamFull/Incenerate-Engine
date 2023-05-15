#pragma once

#include "buffers/VertexBufferObject.h"
#include "handlers/Handler.h"
#include "handlers/PushHandler.h"
#include "pipeline/Pipeline.h"
#include "rendering/Framebuffer.h"
#include "descriptors/DescriptorHandler.h"
#include "APIStructures.h"

namespace engine
{
	namespace graphics
	{
		struct FDispatchParam
		{
			glm::vec3 size;
			bool divideByLocalSizes{ true };
		};

		// Bad solution
		struct FShaderInstance
		{
			std::unique_ptr<CDescriptorHandler> pDescriptorSet;
			std::unordered_map<std::string, std::unique_ptr<CHandler>> mBuffers;
		};

		struct FPipelineParams
		{
			vk::PipelineBindPoint bindPoint{};
			vk::CullModeFlagBits cullMode{};
			vk::FrontFace frontFace{};
			vk::PrimitiveTopology primitiveTopology{};
			std::vector<vk::DynamicState> dynamicStates{};
			bool depthTest{ false };
			bool enableTesselation{ false };
			bool vertexFree{ false };
			bool doubleSided{ false };
			bool useBindlessTextures{ false };
			EVertexType vertexType{};
			EAlphaMode alphaMode{};
			std::string renderStage{};

			// init data
			uint32_t subpass{0};
			uint32_t usages{1};
		};

		class CShaderObject
		{
		public:
			friend class CShaderLoader;
			CShaderObject() = default;
			CShaderObject(CDevice* device);
			~CShaderObject();

			void create(uint32_t subpass = 0);
			void increaseUsage(size_t usages);

			void bind(vk::CommandBuffer& commandBuffer);
			void bindDescriptor(vk::CommandBuffer& commandBuffer);
			void predraw(vk::CommandBuffer& commandBuffer);
			void dispatch(const std::vector<FDispatchParam>& params);
			void dispatch(vk::CommandBuffer& commandBuffer, const std::vector<FDispatchParam>& params);
			void dispatch(const FDispatchParam& param);
			void dispatch(vk::CommandBuffer& commandBuffer, const FDispatchParam& param);

			void addTexture(const std::string& attachment, size_t id, uint32_t mip_level = 0);
			void addTexture(const std::string& attachment, vk::DescriptorImageInfo descriptor);

			vk::DescriptorImageInfo& getTexture(const std::string& attachment);

			const std::unique_ptr<CHandler>& getUniformBuffer(const std::string& name);
			const std::unique_ptr<CPushHandler>& getPushBlock(const std::string& name);

			const std::unique_ptr<CPipeline>& getPipeline() { return pPipeline; }

			const FPipelineParams& getPipelineParams() const { return pipelineParams; }

			vk::PipelineBindPoint getBindPoint() const { return pipelineParams.bindPoint; }
			bool getTesselationFlag() const { return pipelineParams.enableTesselation; }
			const std::string& getStage() const { return pipelineParams.renderStage; }
			const bool isUsesBindlessTextures() const { return pipelineParams.useBindlessTextures; }

			const std::unique_ptr<CShader>& getShader();
		private:
			CDevice* pDevice{ nullptr };
			size_t usageCount{ 0 };
			size_t currentUsage{ 0 };

			std::unique_ptr<CShader> pShader;
			std::unique_ptr<CPipeline> pPipeline;
			std::unordered_map<std::string, vk::DescriptorImageInfo> mTextures;
			std::vector<std::unique_ptr<FShaderInstance>> vInstances;
			
			std::unordered_map<std::string, std::unique_ptr<CPushHandler>> mPushBlocks;

			FPipelineParams pipelineParams{};
		};
	}
}