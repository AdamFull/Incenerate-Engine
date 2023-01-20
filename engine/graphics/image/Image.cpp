#include "Image.h"

#include "APIHandle.h"
#include "buffers/CommandBuffer.h"
#include "buffers/Buffer.h"

using namespace engine::loaders;
using namespace engine::graphics;

CImage::CImage(CDevice* device)
{
    pDevice = device;
}

CImage::~CImage()
{
    auto& vmalloc = pDevice->getVMAAllocator();

    if(_view)
        pDevice->destroy(&_view);
    
    if (_image)
        vmalloc.destroyImage(_image, allocation);
    
    if(_sampler)
        pDevice->destroy(&_sampler);

    pDevice = nullptr;
}

void CImage::create(const std::filesystem::path& srPath, vk::ImageUsageFlags flags, vk::ImageAspectFlags aspect, vk::SamplerAddressMode addressMode, vk::Filter filter)
{
    enableAnisotropy = VK_TRUE;
    std::unique_ptr<FImageCreateInfo> texture;

    CImageLoader::load(srPath.c_str(), texture);

    loadFromMemory(texture, texture->pixFormat, flags, aspect, addressMode, filter);
}

void CImage::create(const std::filesystem::path& srPath, vk::Format format, vk::ImageUsageFlags flags, vk::ImageAspectFlags aspect, vk::SamplerAddressMode addressMode, vk::Filter filter)
{
    enableAnisotropy = VK_TRUE;
    std::unique_ptr<FImageCreateInfo> texture;

    CImageLoader::load(srPath.c_str(), texture);

    texture->pixFormat = format;

    loadFromMemory(texture, texture->pixFormat, flags, aspect, addressMode, filter);
}

void CImage::create(void* pData, const vk::Extent3D& extent, vk::Format format, vk::ImageUsageFlags flags, vk::ImageAspectFlags aspect, vk::SamplerAddressMode addressMode,
    vk::Filter filter, bool mipmap)
{
    auto texture = std::make_unique<FImageCreateInfo>();
    texture->baseWidth = extent.width;
    texture->baseHeight = extent.height;
    texture->baseDepth = 1;
    texture->numDimensions = 2;
    texture->generateMipmaps = mipmap;
    texture->numLevels = mipmap ? static_cast<uint32_t>(std::floor(std::log2((std::max)(extent.width, extent.height)))) + 1 : 1;
    texture->isArray = false;
    texture->numLayers = 1;
    texture->numFaces = 1;
    texture->dataSize = extent.width * extent.height * extent.depth;
    texture->pData = std::make_unique<uint8_t[]>(texture->dataSize);
    texture->mipOffsets.emplace(0, std::vector<size_t>{0});
    std::memcpy(texture->pData.get(), pData, texture->dataSize);

    loadFromMemory(texture, format, flags, aspect, addressMode, filter);
}

void CImage::generateMipmaps(vk::Image& image, uint32_t mipLevels, vk::Format format, uint32_t width, uint32_t height, vk::ImageAspectFlags aspectFlags)
{
    auto& physicalDevice = pDevice->getPhysical();
    log_cerror(physicalDevice, "Trying to generate mipmaps, but physical device is invalid.");
    vk::FormatProperties formatProperties;
    physicalDevice.getFormatProperties(format, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eSampledImageFilterLinear))
        log_error("Texture image format does not support linear blitting!");

    auto cmdBuf = CCommandBuffer(pDevice);
    cmdBuf.create(true, vk::QueueFlagBits::eTransfer);
    auto commandBuffer = cmdBuf.getCommandBuffer();

    int32_t mipWidth = width;
    int32_t mipHeight = height;

    for (uint32_t i = 1; i < mipLevels; i++)
    {
        transitionImageLayout(commandBuffer, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eTransferSrcOptimal, aspectFlags, false, i - 1);
        blitImage(commandBuffer, vk::ImageLayout::eTransferDstOptimal, aspectFlags, i, mipWidth, mipHeight);
        transitionImageLayout(commandBuffer, vk::ImageLayout::eTransferSrcOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, aspectFlags, false, i - 1);

        if (mipWidth > 1)
            mipWidth /= 2;
        if (mipHeight > 1)
            mipHeight /= 2;
    }

    transitionImageLayout(commandBuffer, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, aspectFlags, false, mipLevels - 1);

    cmdBuf.submitIdle();
}

void CImage::initializeTexture(std::unique_ptr<FImageCreateInfo>& info, vk::Format format, vk::ImageUsageFlags flags, vk::ImageAspectFlags aspect, vk::SamplerAddressMode addressMode,
    vk::Filter filter, vk::SampleCountFlagBits samples)
{
    _format = format;
    _addressMode = addressMode;
    _filter = filter;
    _samples = samples;
    _usage = flags;

    //TODO: Add checking for texture type here
    if (!isSupportedDimension(info))
        log_error("Unsupported texture dimension.");

    _extent = vk::Extent3D{ info->baseWidth, info->baseHeight, info->baseDepth };
    _mipLevels = info->numLevels;
    _layerCount = info->numLayers;

    vk::ImageCreateInfo imageInfo{};
    // Select image type
    if (info->baseDepth > 1)
    {
        imageInfo.imageType = vk::ImageType::e3D;
    }
    else
    {
        switch (info->numDimensions)
        {
        case 1:
            imageInfo.imageType = vk::ImageType::e1D;
            break;
        case 2:
            imageInfo.imageType = vk::ImageType::e2D;
            break;
        case 3:
            imageInfo.imageType = vk::ImageType::e3D;
            break;
        }
    }

    imageInfo.extent = _extent;
    imageInfo.mipLevels = _mipLevels;

    if (info->isArray && info->isCubemap)
        imageInfo.arrayLayers = info->numLayers * 6;
    else if (info->isArray)
        imageInfo.arrayLayers = info->numLayers;
    else if (info->isCubemap)
        imageInfo.arrayLayers = 6;
    else
        imageInfo.arrayLayers = 1;

    _instCount = imageInfo.arrayLayers;

    imageInfo.format = _format;
    imageInfo.tiling = vk::ImageTiling::eOptimal;
    imageInfo.initialLayout = _imageLayout;
    imageInfo.usage = flags;
    imageInfo.samples = _samples;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;

    //if (info->isArray && info->isCubemap)
    //    imageInfo.flags = vk::ImageCreateFlagBits::e2DArrayCompatible;
    if (info->isCubemap)
        imageInfo.flags = vk::ImageCreateFlagBits::eCubeCompatible;
    else
        imageInfo.flags = vk::ImageCreateFlags{};

    imageInfo.samples = pDevice->getSamples();

    pDevice->createImage(_image, imageInfo, allocation);

    vk::ImageViewCreateInfo viewInfo{};
    if (info->isArray && info->isCubemap)
        viewInfo.viewType = vk::ImageViewType::eCubeArray;
    else if (info->isArray)
        viewInfo.viewType = vk::ImageViewType::e2DArray;
    else if (info->isCubemap)
        viewInfo.viewType = vk::ImageViewType::eCube;
    else if (info->baseDepth > 1)
        viewInfo.viewType = vk::ImageViewType::e3D;
    else
        viewInfo.viewType = vk::ImageViewType::e2D;

    viewInfo.format = format;
    viewInfo.components = { vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA };
    viewInfo.subresourceRange.aspectMask = aspect;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = _mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = _instCount;
    viewInfo.image = _image;

    vk::Result res = pDevice->create(viewInfo, &_view);
    log_cerror(VkHelper::check(res), "Cannot create image view");

    if (!_sampler)
    {
        bool enableCompare = format == pDevice->getDepthFormat();
        pDevice->createSampler(_sampler, _filter, _addressMode, enableAnisotropy, enableCompare, _mipLevels);
    }
}

void CImage::writeImageData(std::unique_ptr<FImageCreateInfo>& info, vk::Format format, vk::ImageAspectFlags aspect)
{
    vk::DeviceSize imgSize = info->dataSize;

    auto stagingBuffer = CBuffer::MakeStagingBuffer(pDevice, imgSize, 1);
    stagingBuffer->map();
    stagingBuffer->write((void*)info->pData.get());

    transitionImageLayout(vk::ImageLayout::eTransferDstOptimal, aspect);

    std::vector<vk::BufferImageCopy> vRegions;

    if (!info->mipOffsets.empty())
    {
        for (uint32_t layer = 0; layer < _instCount; layer++)
        {
            auto& layer_offsets = info->mipOffsets[layer];
            for (uint32_t level = 0; level < _mipLevels; level++)
            {
                size_t offset = layer_offsets.at(level);
                vk::BufferImageCopy region = {};
                region.imageSubresource.aspectMask = aspect;
                region.imageSubresource.mipLevel = level;
                region.imageSubresource.baseArrayLayer = layer;
                region.imageSubresource.layerCount = _layerCount;
                region.imageExtent.width = info->baseWidth >> level;
                region.imageExtent.height = info->baseHeight >> level;
                region.imageExtent.depth = info->baseDepth;
                region.bufferOffset = offset;
                vRegions.push_back(region);
            }
        }
    }
    else
    {
        vk::BufferImageCopy region = {};
        region.imageSubresource.aspectMask = aspect;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = _instCount;
        region.imageExtent.width = info->baseWidth;
        region.imageExtent.height = info->baseHeight;
        region.imageExtent.depth = info->baseDepth;
        region.bufferOffset = 0;
        vRegions.push_back(region);
    }

    auto buffer = stagingBuffer->getBuffer();
    pDevice->copyBufferToImage(buffer, _image, vRegions);

    if (info->generateMipmaps)
        generateMipmaps(_image, _mipLevels, format, _extent.width, _extent.height, aspect);
    else
        transitionImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal, aspect);
}

void CImage::loadFromMemory(std::unique_ptr<FImageCreateInfo>& info, vk::Format format, vk::ImageUsageFlags flags,
    vk::ImageAspectFlags aspect, vk::SamplerAddressMode addressMode, vk::Filter filter)
{
    initializeTexture(info, format, flags, aspect, addressMode, filter, vk::SampleCountFlagBits::e1);
    writeImageData(info, format, aspect);
    updateDescriptor();
}

void CImage::transitionImageLayout(vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips)
{
    auto cmdBuf = CCommandBuffer(pDevice);
    cmdBuf.create(true, vk::QueueFlagBits::eTransfer);
    auto commandBuffer = cmdBuf.getCommandBuffer();
    transitionImageLayout(commandBuffer, _imageLayout, newLayout, aspectFlags, use_mips);
    cmdBuf.submitIdle();
}

void CImage::transitionImageLayout(vk::CommandBuffer& commandBuffer, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips , uint32_t base_mip)
{
    transitionImageLayout(commandBuffer, _imageLayout, newLayout, aspectFlags, use_mips, base_mip);
}

void CImage::transitionImageLayout(vk::CommandBuffer& commandBuffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::ImageAspectFlags aspectFlags, bool use_mips, uint32_t base_mip)
{
    std::vector<vk::ImageMemoryBarrier> vBarriers;
    vk::ImageMemoryBarrier barrier{};
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = aspectFlags;
    barrier.subresourceRange.baseMipLevel = base_mip;
    barrier.subresourceRange.levelCount = use_mips ? _mipLevels : 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = _instCount;
    vBarriers.push_back(barrier);

    pDevice->transitionImageLayout(commandBuffer, _image, vBarriers, oldLayout, newLayout);
    _imageLayout = newLayout;
}

void CImage::transitionImageLayoutGraphics(vk::CommandBuffer& commandBuffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
    auto queue_indices = pDevice->findQueueFamilies();

    std::vector<vk::ImageMemoryBarrier> vBarriers;
    vk::ImageMemoryBarrier barrier{};
    barrier.srcQueueFamilyIndex = queue_indices.graphicsFamily.value_or(VK_QUEUE_FAMILY_IGNORED);
    barrier.dstQueueFamilyIndex = queue_indices.graphicsFamily.value_or(VK_QUEUE_FAMILY_IGNORED);
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.image = _image;
    barrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead | vk::AccessFlagBits::eMemoryWrite;
    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = _mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = _instCount;
    vBarriers.push_back(barrier);

    pDevice->transitionImageLayout(commandBuffer, _image, vBarriers, oldLayout, newLayout);
    _imageLayout = newLayout;
}

bool CImage::isSupportedDimension(std::unique_ptr<FImageCreateInfo>& info)
{
    auto& physicalDevice = pDevice->getPhysical();
    log_cerror(physicalDevice, "Trying to check supported dibension, but physical device is invalid.");
    vk::PhysicalDeviceProperties devprops;
    physicalDevice.getProperties(&devprops);

    if (info->isCubemap)
    {
        return !(info->baseWidth > devprops.limits.maxImageDimensionCube ||
            info->baseHeight > devprops.limits.maxImageDimensionCube ||
            info->baseDepth > devprops.limits.maxImageDimensionCube);
    }

    if (info->isArray)
    {
        return !(info->numLayers > devprops.limits.maxImageArrayLayers ||
            info->numFaces > devprops.limits.maxImageArrayLayers);
    }

    if (info->numDimensions == 1)
    {
        return !(info->baseWidth > devprops.limits.maxImageDimension1D ||
            info->baseHeight > devprops.limits.maxImageDimension1D ||
            info->baseDepth > devprops.limits.maxImageDimension1D);
    }

    if (info->numDimensions == 2)
    {
        return !(info->baseWidth > devprops.limits.maxImageDimension2D ||
            info->baseHeight > devprops.limits.maxImageDimension2D ||
            info->baseDepth > devprops.limits.maxImageDimension2D);
    }

    if (info->numDimensions == 3)
    {
        return !(info->baseWidth > devprops.limits.maxImageDimension3D ||
            info->baseHeight > devprops.limits.maxImageDimension3D ||
            info->baseDepth > devprops.limits.maxImageDimension3D);
    }

    return true;
}

void CImage::blitImage(vk::CommandBuffer& commandBuffer, vk::ImageLayout dstLayout, vk::ImageAspectFlags aspectFlags, uint32_t level, int32_t mipWidth, int32_t mipHeight)
{
    vk::ImageBlit blit{};
    blit.srcOffsets[0] = vk::Offset3D{ 0, 0, 0 };
    blit.srcOffsets[1] = vk::Offset3D{ mipWidth, mipHeight, 1 };
    blit.srcSubresource.aspectMask = aspectFlags;
    blit.srcSubresource.mipLevel = level - 1;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = 1;
    blit.dstOffsets[0] = vk::Offset3D{ 0, 0, 0 };
    blit.dstOffsets[1] = vk::Offset3D{ mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
    blit.dstSubresource.aspectMask = aspectFlags;
    blit.dstSubresource.mipLevel = level;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;

    commandBuffer.blitImage(_image, _imageLayout, _image, dstLayout, 1, &blit, vk::Filter::eLinear);
}

void CImage::copyImageToDst(vk::CommandBuffer& commandBuffer, std::shared_ptr<CImage>& pDst, vk::ImageCopy& region, vk::ImageLayout dstLayout)
{
    pDevice->copyTo(commandBuffer, _image, pDst->_image, _imageLayout, dstLayout, region);
}

void CImage::updateDescriptor()
{
    _descriptor.sampler = _sampler;
    _descriptor.imageView = _view;
    _descriptor.imageLayout = _imageLayout;
}

vk::DescriptorImageInfo& CImage::getDescriptor()
{
    if (_imageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal && (_usage & vk::ImageUsageFlagBits::eInputAttachment))
    {
        _imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        updateDescriptor();
    }
    return _descriptor;
}