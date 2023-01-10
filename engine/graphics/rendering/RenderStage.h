#pragma once
#include "Framebuffer.h"

namespace engine
{
	namespace graphics
	{
		struct FCIViewport
		{
			vk::Offset2D offset;
			vk::Extent2D extent;
		};

		struct FCIImage
		{
			FCIImage(const std::string& name, vk::Format fmt, vk::ImageUsageFlags usg, EImageType tp = EImageType::e2D, uint32_t lrs = 1) :
				srName(name), format(fmt), usage(usg), type(tp), layers(lrs)
			{

			}

			std::string srName;
			vk::Format format;
			vk::ImageUsageFlags usage;
			EImageType type{ EImageType::e2D };
			uint32_t layers{ 1 };
		};

		struct FCIDependencyDesc
		{
			FCIDependencyDesc(uint32_t spass, vk::PipelineStageFlags smask, vk::AccessFlags amask) :
				subpass(spass), stageMask(smask), accessMask(amask)
			{

			}

			uint32_t subpass;
			vk::PipelineStageFlags stageMask;
			vk::AccessFlags accessMask;
		};

		struct FCIDependency
		{
			FCIDependency(FCIDependencyDesc srcDesc, FCIDependencyDesc dstDesc, vk::DependencyFlags flags = vk::DependencyFlagBits::eByRegion) :
				src(srcDesc), dst(dstDesc), depFlags(flags)
			{

			}

			FCIDependencyDesc src;
			FCIDependencyDesc dst;
			vk::DependencyFlags depFlags{ vk::DependencyFlagBits::eByRegion };
		};

		struct FCIStage
		{
			std::string srName;
			ERenderStageAvaliableFlagBits eFlag;
			FCIViewport viewport;
			bool bFlipViewport{ false };
			bool bIgnoreRecreation{ false };
			bool bViewportDependent{ false };
			std::vector<FCIImage> vImages;
			std::vector<std::string> vOutputs;
			std::vector<std::string> vDescriptions;
			std::vector<FCIDependency> vDependencies;
		};

		struct FCIRenderSystem
		{
			std::string srName;
			std::vector<FCIStage> vStages;
		};

		class CRenderStage
		{
		public:
			CRenderStage(CDevice* device);
			void create(const FCIStage& createInfo);
			void reCreate(const FCIStage& createInfo);

			void begin(vk::CommandBuffer& commandBuffer);
			void end(vk::CommandBuffer& commandBuffer);

			const std::unique_ptr<CFramebuffer>& getFramebuffer() const;
			ERenderStageAvaliableFlagBits getStageFlag();
		private:
			CDevice* pDevice{ nullptr };
			FCIStage stageCI;
			std::unique_ptr<CFramebuffer> pFramebuffer;
		};
	}
}