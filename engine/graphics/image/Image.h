#pragma once

#include "ImageLoader.h"
#include <vk_mem_alloc.h>

#include <filesystem>

namespace engine
{
	namespace graphics
	{
		class CDevice;
		class CImage
		{
		public:
			CImage() = default;
			CImage(CDevice* device);
			virtual ~CImage();

			void create(const std::filesystem::path& srPath, vk::ImageUsageFlags flags = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
				vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor, vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eRepeat,
				vk::Filter filter = vk::Filter::eLinear);

			void create(void* pData, const vk::Extent3D& extent, vk::Format format = vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlags flags = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
				vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor, vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eRepeat,
				vk::Filter filter = vk::Filter::eNearest, bool mipmap = false);

			void generateMipmaps(vk::Image& image, uint32_t mipLevels, vk::Format format, uint32_t width, uint32_t height, vk::ImageAspectFlags aspectFlags);

			void initializeTexture(std::unique_ptr<FImageCreateInfo>& info, vk::Format format, vk::ImageUsageFlags flags, vk::ImageAspectFlags aspect, vk::SamplerAddressMode addressMode,
				vk::Filter filter, vk::SampleCountFlagBits samples);

			void writeImageData(std::unique_ptr<FImageCreateInfo>& info, vk::Format format, vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor);
			void loadFromMemory(std::unique_ptr<FImageCreateInfo>& info, vk::Format format,
				vk::ImageUsageFlags flags = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
				vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor, vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eRepeat,
				vk::Filter filter = vk::Filter::eLinear);

			void transitionImageLayout(vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips = true);
			void transitionImageLayout(vk::CommandBuffer& commandBuffer, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips = true, uint32_t base_mip = 0);
			void transitionImageLayout(vk::CommandBuffer& commandBuffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips = true, uint32_t base_mip = 0);

			bool isSupportedDimension(std::unique_ptr<FImageCreateInfo>& info);

			void blitImage(vk::CommandBuffer& commandBuffer, vk::ImageLayout dstLayout, vk::ImageAspectFlags aspectFlags, uint32_t level, int32_t mipWidth, int32_t mipHeight);

			void copyImageToDst(vk::CommandBuffer& commandBuffer, std::shared_ptr<CImage>& pDst, vk::ImageCopy& region, vk::ImageLayout dstLayout);

			void updateDescriptor();

			vk::DescriptorImageInfo& getDescriptor();

			void setImageLayout(vk::ImageLayout layout) { _imageLayout = layout; }

		protected:
			CDevice* pDevice{ nullptr };

			vk::Image _image{ VK_NULL_HANDLE };
			vk::ImageView _view{ VK_NULL_HANDLE };
			vk::Sampler _sampler{ VK_NULL_HANDLE };
			vk::DescriptorSet descriptorSet{ VK_NULL_HANDLE };
			VmaAllocation allocation{ VK_NULL_HANDLE };

			vk::DescriptorImageInfo _descriptor;

			vk::Extent3D _extent;
			vk::SampleCountFlagBits _samples;
			vk::ImageUsageFlags _usage;
			uint32_t _mipLevels;
			uint32_t _instCount;
			uint32_t _layerCount;
			vk::Format _format;
			bool enableAnisotropy{ false };

			vk::Filter _filter{ vk::Filter::eLinear };
			vk::SamplerAddressMode _addressMode;
			vk::ImageLayout _imageLayout{ vk::ImageLayout::eUndefined };
		};
	}
}