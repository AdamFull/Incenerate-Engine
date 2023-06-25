#pragma once

#include "Image.h"

namespace engine
{
	namespace graphics
	{
		class CImage3D : public CImage
		{
		public:
			CImage3D(CDevice* device);
			virtual ~CImage3D() = default;

			void create(const vk::Extent3D& extent, vk::Format format = vk::Format::eR8Unorm, vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal,
				vk::ImageUsageFlags usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled, vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eColor,
				vk::Filter filter = vk::Filter::eLinear, vk::SamplerAddressMode addressMode = vk::SamplerAddressMode::eClampToEdge, vk::SampleCountFlagBits samples = vk::SampleCountFlagBits::e1,
				bool instantLayoutTransition = true);
		};
	}
}