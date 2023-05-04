#include "Image.h"

#include "APIHandle.h"
#include "buffers/CommandBuffer.h"
#include "buffers/Buffer.h"

using namespace engine::graphics;

CImage::CImage(CDevice* device)
{
    pDevice = device;
}

CImage::~CImage()
{
    auto& vmalloc = pDevice->getVMAAllocator();

    for(auto& view : _views)
        pDevice->destroy(&view);
    
    if (_image)
        vmalloc.destroyImage(_image, allocation);
    
    if(_sampler)
        pDevice->destroy(&_sampler);

    pDevice = nullptr;
}

void CImage::create(std::unique_ptr<FImageCreateInfo>& info, vk::ImageUsageFlags flags, vk::ImageAspectFlags aspect, vk::SamplerAddressMode addressMode, vk::Filter filter)
{
    enableAnisotropy = VK_TRUE;
    loadFromMemory(info, info->pixFormat, flags, aspect, addressMode, filter);
}

void CImage::create(const std::string& srPath, vk::ImageUsageFlags flags, vk::ImageAspectFlags aspect, vk::SamplerAddressMode addressMode, vk::Filter filter)
{
    enableAnisotropy = VK_TRUE;
    std::unique_ptr<FImageCreateInfo> texture;

    auto& loader = pDevice->getAPI()->getImageLoader();
    loader->load(srPath, texture);

    loadFromMemory(texture, texture->pixFormat, flags, aspect, addressMode, filter);
}

void CImage::create(const std::string& srPath, vk::Format format, vk::ImageUsageFlags flags, vk::ImageAspectFlags aspect, vk::SamplerAddressMode addressMode, vk::Filter filter)
{
    enableAnisotropy = VK_TRUE;
    std::unique_ptr<FImageCreateInfo> texture;

    auto& loader = pDevice->getAPI()->getImageLoader();
    loader->load(srPath, texture);

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
    cmdBuf.create(true, vk::QueueFlagBits::eGraphics);
    auto commandBuffer = cmdBuf.getCommandBuffer();

    vk::ImageMemoryBarrier2 barrier{};
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = _image;
    barrier.subresourceRange.aspectMask = aspectFlags;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = _instCount;

    int32_t mipWidth = width;
    int32_t mipHeight = height;

    for (uint32_t i = 1; i < mipLevels; i++)
    {
        barrier.subresourceRange.levelCount = i - 1;
        barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
        pDevice->transitionImageLayoutTransfer(commandBuffer, barrier);

        blitImage(commandBuffer, vk::ImageLayout::eTransferDstOptimal, aspectFlags, i, mipWidth, mipHeight);

        barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
        barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        pDevice->transitionImageLayoutGraphics(commandBuffer, barrier);

        if (mipWidth > 1)
            mipWidth /= 2;
        if (mipHeight > 1)
            mipHeight /= 2;
    }

    barrier.subresourceRange.levelCount = mipLevels - 1;
    barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    pDevice->transitionImageLayoutGraphics(commandBuffer, barrier);

    _imageLayout = barrier.newLayout;

    cmdBuf.submitIdle();
}

void CImage::initializeTexture(std::unique_ptr<FImageCreateInfo>& info, vk::Format format, vk::ImageUsageFlags flags, vk::ImageAspectFlags aspect, vk::SamplerAddressMode addressMode,
    vk::Filter filter, vk::SampleCountFlagBits samples)
{
    _aspectMask = aspect;
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

    vma::MemoryUsage memoryUsage{ vma::MemoryUsage::eUnknown };
    vk::ImageTiling tiling{ vk::ImageTiling::eOptimal };
    //if ((flags & vk::ImageUsageFlagBits::eTransferSrc) || (flags & vk::ImageUsageFlagBits::eTransferDst))
    //{
    //    memoryUsage = vma::MemoryUsage::eGpuToCpu;
    //    tiling = vk::ImageTiling::eLinear;
    //}

    imageInfo.format = _format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = _imageLayout;
    imageInfo.usage = flags;
    imageInfo.samples = _samples;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;

    if (info->isCubemap)
        imageInfo.flags = vk::ImageCreateFlagBits::eCubeCompatible;
    else
        imageInfo.flags = vk::ImageCreateFlags{};

    imageInfo.samples = pDevice->getSamples();

    pDevice->createImage(_image, imageInfo, allocation, memoryUsage);

    vk::ImageView imageView;

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

    vk::Result res = pDevice->create(viewInfo, &imageView);
    log_cerror(VkHelper::check(res), "Cannot create image view");
    _views.emplace_back(imageView);

    viewInfo.subresourceRange.levelCount = 1;

    for (uint32_t mip = 1; mip < _mipLevels; mip++)
    {
        viewInfo.subresourceRange.baseMipLevel = mip;
        vk::Result res = pDevice->create(viewInfo, &imageView);
        log_cerror(VkHelper::check(res), "Cannot create image view");
        _views.emplace_back(imageView);
    }

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

    vk::ImageMemoryBarrier2 barrier{};
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.oldLayout = _imageLayout;
    barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
    barrier.image = _image;
    barrier.subresourceRange.aspectMask = aspect;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = _mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = _instCount;
    _imageLayout = barrier.newLayout;

    auto tcmdBuf = CCommandBuffer(pDevice);
    tcmdBuf.create(true, vk::QueueFlagBits::eTransfer);
    auto transferBuffer = tcmdBuf.getCommandBuffer();

    pDevice->transitionImageLayoutTransfer(transferBuffer, barrier);

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
    pDevice->copyBufferToImage(transferBuffer, buffer, _image, vRegions);

    barrier.srcQueueFamilyIndex = pDevice->getQueueFamilyIndex(family::transfer);
    barrier.dstQueueFamilyIndex = pDevice->getQueueFamilyIndex(family::graphics);
    barrier.oldLayout = barrier.newLayout;

    // Transfer family ownership
    pDevice->transitionImageLayoutTransfer(transferBuffer, barrier);

    tcmdBuf.submitIdle();

    if (info->generateMipmaps && _mipLevels > 1)
        generateMipmaps(_image, _mipLevels, format, _extent.width, _extent.height, aspect);
    else
    {
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        _imageLayout = barrier.newLayout;
        pDevice->transitionImageLayoutGraphics(barrier);
    }
        
}

void CImage::loadFromMemory(std::unique_ptr<FImageCreateInfo>& info, vk::Format format, vk::ImageUsageFlags flags,
    vk::ImageAspectFlags aspect, vk::SamplerAddressMode addressMode, vk::Filter filter)
{
    initializeTexture(info, format, flags, aspect, addressMode, filter, vk::SampleCountFlagBits::e1);
    writeImageData(info, format, aspect);
    updateDescriptor();
    loaded = true;
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

void CImage::copyTo(vk::CommandBuffer& commandBuffer, vk::Image& src, vk::Image& dst, vk::ImageLayout srcLayout, vk::ImageLayout dstLayout, vk::ImageCopy& region)
{
    commandBuffer.copyImage(src, srcLayout, dst, dstLayout, 1, &region);
}

void CImage::copyImageToDst(vk::CommandBuffer& commandBuffer, std::shared_ptr<CImage>& pDst, vk::ImageCopy& region, vk::ImageLayout dstLayout)
{
    copyTo(commandBuffer, _image, pDst->_image, _imageLayout, dstLayout, region);
}

void CImage::updateDescriptor(uint32_t mip_level)
{
    _descriptor.sampler = _sampler;
    _descriptor.imageView = _views.at(mip_level);
    _descriptor.imageLayout = _imageLayout;
}

vk::DescriptorImageInfo& CImage::getDescriptor(uint32_t mip_level)
{
    if (_imageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal && (_usage & vk::ImageUsageFlagBits::eInputAttachment))
        _imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    
    updateDescriptor(mip_level);

    return _descriptor;
}