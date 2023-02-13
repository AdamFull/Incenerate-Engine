#include "Image2D.h"

#include "APIHandle.h"

using namespace engine::graphics;
using namespace engine::loaders;

CImage2D::CImage2D(CDevice* device)
{
    pDevice = device;
}

void CImage2D::create(const vk::Extent2D& extent, vk::Format format, vk::ImageLayout layout, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspect,
	vk::Filter filter, vk::SamplerAddressMode addressMode, vk::SampleCountFlagBits samples, bool instantLayoutTransition, bool anisotropic, bool mipmaps, 
    uint32_t levelCount)
{
    uint32_t mip_levels{ 1 };
    if (mipmaps)
        mip_levels = levelCount == 1 ? static_cast<uint32_t>(std::floor(std::log2(std::max(extent.width, extent.height)))) + 1 : levelCount;

    enableAnisotropy = anisotropic;
    auto texture = std::make_unique<FImageCreateInfo>();
    texture->baseWidth = extent.width;
    texture->baseHeight = extent.height;
    texture->baseDepth = 1;
    texture->numDimensions = 2;
    texture->generateMipmaps = mipmaps;
    texture->numLevels = mip_levels;
    texture->isArray = false;
    texture->numLayers = 1;
    texture->numFaces = 1;
    texture->dataSize = extent.width * extent.height * (texture->baseDepth > 1 ? texture->baseDepth : 4);

    initializeTexture(texture, format, usage, aspect, addressMode, filter, samples);

    if (instantLayoutTransition)
        transitionImageLayout(layout, aspect, mipmaps);
    else
        setImageLayout(layout);
    updateDescriptor();
    loaded = true;
}