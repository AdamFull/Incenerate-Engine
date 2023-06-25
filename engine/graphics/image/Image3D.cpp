#include "Image3D.h"

#include "APIHandle.h"

using namespace engine::graphics;

CImage3D::CImage3D(CDevice* device)
{
	pDevice = device;
}

void CImage3D::create(const vk::Extent3D& extent, vk::Format format, vk::ImageLayout layout, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspect, vk::Filter filter, vk::SamplerAddressMode addressMode, vk::SampleCountFlagBits samples, bool instantLayoutTransition)
{
	auto texture = std::make_unique<FImageCreateInfo>();
	texture->baseWidth = extent.width;
	texture->baseHeight = extent.height;
	texture->baseDepth = extent.depth;
	texture->numDimensions = 3;
	texture->generateMipmaps = false;
	texture->numLevels = 1;
	texture->isArray = false;
	texture->numLayers = 1;
	texture->numFaces = 1;
	texture->dataSize = extent.width * extent.height * (texture->baseDepth > 1 ? texture->baseDepth : 4);

	initializeTexture(texture, format, usage, aspect, addressMode, filter, samples);

	if (instantLayoutTransition)
	{
		vk::ImageMemoryBarrier2 barrier{};
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.oldLayout = _imageLayout;
		barrier.newLayout = layout;
		barrier.subresourceRange.aspectMask = aspect;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = _instCount;
		barrier.image = _image;
		_imageLayout = layout;

		pDevice->transitionImageLayoutGraphics(barrier);
	}
	else
		setImageLayout(layout);

	updateDescriptor();
	loaded = true;
}