#include "ImageCubemapArray.h"

#include "APIHandle.h"

using namespace engine::graphics;

CImageCubemapArray::CImageCubemapArray(CDevice* device)
{
    pDevice = device;
}

void CImageCubemapArray::create(uint32_t layers, const vk::Extent2D& extent, vk::Format format, vk::ImageLayout layout,
	vk::ImageUsageFlags usage, vk::ImageAspectFlags aspect, vk::Filter filter, vk::SamplerAddressMode addressMode, vk::SampleCountFlagBits samples,
	bool instantLayoutTransition, bool anisotropic, bool mipmaps)
{
    enableAnisotropy = anisotropic;
    auto texture = std::make_unique<FImageCreateInfo>();
    texture->baseWidth = extent.width;
    texture->baseHeight = extent.height;
    texture->baseDepth = 1;
    texture->numDimensions = 2;
    texture->generateMipmaps = mipmaps;
    texture->numLevels = mipmaps ? static_cast<uint32_t>(std::floor(std::log2(std::max(extent.width, extent.height)))) + 1 : 1;
    texture->isArray = true;
    texture->isCubemap = true;
    texture->numLayers = layers;
    texture->numFaces = 6;
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
        barrier.subresourceRange.levelCount = mipmaps ? _mipLevels : 1;
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