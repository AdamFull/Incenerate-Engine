#pragma once

#include "handlers/Handler.h"
#include "pipeline/Pipeline.h"
#include "descriptors/DescriptorHandler.h"
#include "APIStructures.h"

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

		class CImage;
		class CShaderObject
		{
		public:
			friend class CShaderLoader;
			CShaderObject() = default;

			void create();
			void reCreate();
			void update();

			void bind(vk::CommandBuffer& commandBuffer);

			void addTexture(const std::string& attachment, vk::DescriptorImageInfo& descriptor);
			void addTexture(const std::string& attachment, std::shared_ptr<CImage>& pTexture);
			void addTexture(const std::string& attachment, utl::weak_ptr<CImage>& pTexture);

			vk::DescriptorImageInfo& getTexture(const std::string& attachment);

			std::unique_ptr<CHandler>& getUniformBuffer(const std::string& name);
			std::map<std::string, utl::scope_ptr<CHandler>>& getUniformBuffers();
			utl::scope_ptr<CDescriptorHandler>& getDescriptorSet();

			std::unique_ptr<CPipeline>& getPipeline() { return pPipeline; }

			vk::CullModeFlagBits getCullMode() { return programCI.cullMode; }
			vk::FrontFace getFrontFace() { return programCI.frontFace; }
			bool getDepthTestFlag() { return programCI.depthTest; }
			std::vector<vk::DynamicState>& getDynamicStateEnables() { return programCI.dynamicStates; }
			bool getTesselationFlag() { return programCI.tesselation; }

			const std::unique_ptr<CShader>& getShader();
		private:
			std::unique_ptr<CShader> pShader;
			std::unique_ptr<CPipeline> pPipeline;

			uint32_t currentInstance{ 0 }, instances{ 1 };
			bool bIsCreated{ false }, bIsReCreated{ false };

			std::unique_ptr<CHandler> pEmptyHandler{ nullptr };
			std::vector<utl::scope_ptr<FSOInstance>> vInstances;
			std::map<std::string, vk::DescriptorImageInfo> mTextures;

			FProgramCreateInfo programCI;
		};
	}
}