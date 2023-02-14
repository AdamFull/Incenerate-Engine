#pragma once

#include <vma/vk_mem_alloc.hpp>
#include "loaders/ImageLoader.h"

namespace engine
{
	namespace graphics
	{
		class CImage
		{
		public:
			CImage() = default;
			CImage(CDevice* device);
			virtual ~CImage();

			void create(std::unique_ptr<loaders::FImageCreateInfo>& info, vk::ImageUsageFlags flags = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
				vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor, vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eRepeat,
				vk::Filter filter = vk::Filter::eLinear);

			void create(const std::filesystem::path& srPath, vk::ImageUsageFlags flags = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
				vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor, vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eRepeat,
				vk::Filter filter = vk::Filter::eLinear);

			void create(const std::filesystem::path& srPath, vk::Format format, vk::ImageUsageFlags flags = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
				vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor, vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eRepeat,
				vk::Filter filter = vk::Filter::eLinear);

			void create(void* pData, const vk::Extent3D& extent, vk::Format format = vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlags flags = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
				vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor, vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eRepeat,
				vk::Filter filter = vk::Filter::eNearest, bool mipmap = false);

			void generateMipmaps(vk::Image& image, uint32_t mipLevels, vk::Format format, uint32_t width, uint32_t height, vk::ImageAspectFlags aspectFlags);

			void initializeTexture(std::unique_ptr<loaders::FImageCreateInfo>& info, vk::Format format, vk::ImageUsageFlags flags, vk::ImageAspectFlags aspect, vk::SamplerAddressMode addressMode,
				vk::Filter filter, vk::SampleCountFlagBits samples);

			void writeImageData(std::unique_ptr<loaders::FImageCreateInfo>& info, vk::Format format, vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor);
			void loadFromMemory(std::unique_ptr<loaders::FImageCreateInfo>& info, vk::Format format,
				vk::ImageUsageFlags flags = vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
				vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor, vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eRepeat,
				vk::Filter filter = vk::Filter::eLinear);

			bool isSupportedDimension(std::unique_ptr<loaders::FImageCreateInfo>& info);

			void blitImage(vk::CommandBuffer& commandBuffer, vk::ImageLayout dstLayout, vk::ImageAspectFlags aspectFlags, uint32_t level, int32_t mipWidth, int32_t mipHeight);

			static void copyTo(vk::CommandBuffer& commandBuffer, vk::Image& src, vk::Image& dst, vk::ImageLayout srcLayout, vk::ImageLayout dstLayout, vk::ImageCopy& region);
			void copyImageToDst(vk::CommandBuffer& commandBuffer, std::shared_ptr<CImage>& pDst, vk::ImageCopy& region, vk::ImageLayout dstLayout);

			void updateDescriptor(uint32_t mip_level = 0);

			vk::DescriptorImageInfo& getDescriptor(uint32_t mip_level = 0);

			void setImageLayout(vk::ImageLayout layout) { _imageLayout = layout; }

			const uint32_t getMipLevels() const { return _mipLevels; }
			const uint32_t getLayers() const { return _layerCount; }
			const vk::Format getFormat() const { return _format; }
			vk::Image& getImage() { return _image; }
			const vk::Extent3D& getExtent() const { return _extent; }
			const vk::ImageLayout getLayout() const { return _imageLayout; }
			const vk::ImageAspectFlags& getAspectMask() const { return _aspectMask; }
			const bool isLoaded() const { return loaded; }

		protected:
			CDevice* pDevice{ nullptr };

			vk::Image _image{ VK_NULL_HANDLE };
			std::vector<vk::ImageView> _views;
			vk::Sampler _sampler{ VK_NULL_HANDLE };
			vk::DescriptorSet descriptorSet{ VK_NULL_HANDLE };
			vma::Allocation allocation{ VK_NULL_HANDLE };

			vk::DescriptorImageInfo _descriptor;

			vk::Extent3D _extent;
			vk::SampleCountFlagBits _samples;
			vk::ImageUsageFlags _usage;
			vk::ImageAspectFlags _aspectMask;
			uint32_t _mipLevels;
			uint32_t _instCount;
			uint32_t _layerCount;
			vk::Format _format;
			bool enableAnisotropy{ false };
			bool loaded{ false };

			vk::Filter _filter{ vk::Filter::eLinear };
			vk::SamplerAddressMode _addressMode;
			vk::ImageLayout _imageLayout{ vk::ImageLayout::eUndefined };
		};
	}
}