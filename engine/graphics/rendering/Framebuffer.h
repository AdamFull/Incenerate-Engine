#pragma once

#include <vulkan/vulkan.hpp>

#include <fifo_map.hpp>
#include <utility/logger/logger.h>
#include "EngineStructures.h"

namespace engine
{
	namespace graphics
	{
		struct FFramebufferAttachmentInfo
		{
			vk::Format format;
			vk::ImageUsageFlags usageFlags;
			EImageType eType;
			uint32_t reference;
			uint32_t layers;
		};

		class CImage;
		class CDevice;

		class CFramebuffer
		{
		public:
			CFramebuffer(CDevice* device);
			~CFramebuffer();

			void create();
			void reCreate();

			void begin(vk::CommandBuffer& commandBuffer);
			void end(vk::CommandBuffer& commandBuffer);


			void addInputReference(uint32_t index, const std::vector<std::string>& vref);
			void addOutputReference(uint32_t index, const std::vector<std::string>& vref);

			void addDescription(uint32_t subpass, const std::string& depthReference = "");
			void addSubpassDependency(uint32_t src, uint32_t dst, vk::PipelineStageFlags srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
				vk::PipelineStageFlags dstStageMask = vk::PipelineStageFlagBits::eFragmentShader, vk::AccessFlags srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite,
				vk::AccessFlags dstAccessMask = vk::AccessFlagBits::eMemoryRead, vk::DependencyFlags depFlags = vk::DependencyFlagBits::eByRegion);

			const vk::Rect2D& getRenderArea() const { return renderArea; }
			void setRenderArea(int32_t offset_x, int32_t offset_y, uint32_t width, uint32_t height);
			void setRenderArea(vk::Offset2D offset, vk::Extent2D extent);
			void setRenderArea(vk::Rect2D&& area);

			void setFlipViewport(vk::Bool32 value) { flipViewport = value; }

			vk::RenderPass& getRenderPass() { return renderPass; }

			void addImage(const std::string& name, vk::Format format, vk::ImageUsageFlags usageFlags, EImageType eImageType = EImageType::e2D, uint32_t layers = 1);

			vk::Framebuffer& getFramebuffer(uint32_t index) { return vFramebuffers[index]; }
			vk::Framebuffer& getCurrentFramebuffer();
			std::unordered_map<std::string, size_t>& getImages(uint32_t index) { return mFramebufferImages[index]; }
			std::unordered_map<std::string, size_t>& getCurrentImages();
			size_t getDepthImage() { return depthImageIDX; }
			const std::vector<vk::AttachmentDescription>& getAttachmentDescriptions() const { return vAttachDesc; };

			vk::SubpassDescription& getDescription() { return vSubpassDesc.front(); }

			void createRenderPass();
			void createFramebuffer();
			std::unique_ptr<CImage> createImage(const FFramebufferAttachmentInfo& attachment, vk::Extent2D extent);

			static bool isColorAttachment(vk::ImageUsageFlags usageFlags) { return static_cast<bool>(usageFlags & vk::ImageUsageFlagBits::eColorAttachment); }
			static bool isDepthAttachment(vk::ImageUsageFlags usageFlags) { return static_cast<bool>(usageFlags & vk::ImageUsageFlagBits::eDepthStencilAttachment); }
			static bool isSampled(vk::ImageUsageFlags usageFlags) { return static_cast<bool>(usageFlags & vk::ImageUsageFlagBits::eSampled); }
			static bool isTransferSrc(vk::ImageUsageFlags usageFlags) { return static_cast<bool>(usageFlags & vk::ImageUsageFlagBits::eTransferSrc); }
			static bool isStorage(vk::ImageUsageFlags usageFlags) { return static_cast<bool>(usageFlags & vk::ImageUsageFlagBits::eStorage); }
			static bool isInputAttachment(vk::ImageUsageFlags usageFlags) { return static_cast<bool>(usageFlags & vk::ImageUsageFlagBits::eInputAttachment); }
		private:
			void clearImages();
		private:
			CDevice* pDevice{ nullptr };
			vk::Rect2D renderArea;

			vk::RenderPass renderPass{ nullptr };
			bool flipViewport{ false };
			std::vector<vk::AttachmentDescription> vAttachDesc;
			std::vector<vk::SubpassDescription> vSubpassDesc;
			std::vector<vk::SubpassDependency> vSubpassDep;
			std::vector<vk::ClearValue> vClearValues;
			std::map<uint32_t, std::vector<vk::AttachmentReference>> mInputReferences;
			std::map<uint32_t, std::vector<vk::AttachmentReference>> mOutputReferences;
			std::map<std::string, vk::AttachmentReference> mDepthReference;

			std::vector<vk::Framebuffer> vFramebuffers;
			nlohmann::fifo_map<std::string, FFramebufferAttachmentInfo> mFbAttachments;
			std::map<uint32_t, std::unordered_map<std::string, size_t>> mFramebufferImages;
			size_t depthImageIDX{ invalid_index };
		};
	}
}