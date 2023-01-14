#include "ImageCubemap.h"

#include "APIHandle.h"

using namespace engine::graphics;
using namespace engine::loaders;

CImageCubemap::CImageCubemap(CDevice* device)
{
    pDevice = device;
}

void CImageCubemap::create(const vk::Extent2D& extent, vk::Format format, vk::ImageLayout layout, vk::ImageUsageFlags usage,
	vk::ImageAspectFlags aspect, vk::Filter filter, vk::SamplerAddressMode addressMode, vk::SampleCountFlagBits samples,
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
    texture->isArray = false;
    texture->isCubemap = true;
    texture->numLayers = 1;
    texture->numFaces = 6;
    texture->dataSize = extent.width * extent.height * (texture->baseDepth > 1 ? texture->baseDepth : 4);

    initializeTexture(texture, format, usage, aspect, addressMode, filter, samples);

    if (instantLayoutTransition)
        transitionImageLayout(layout, aspect, mipmaps);
    else
        setImageLayout(layout);
    updateDescriptor();
}