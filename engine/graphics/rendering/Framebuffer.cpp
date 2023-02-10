#include "Framebuffer.h"

#include "Engine.h"
#include "APIHandle.h"
#include "image/Image2D.h"
#include "image/Image2DArray.h"
#include "image/ImageCubemap.h"
#include "image/ImageCubemapArray.h"

using namespace engine::graphics;

CFramebuffer::CFramebuffer(CDevice* device)
{
    pDevice = device;
}

CFramebuffer::~CFramebuffer()
{
    if (renderPass)
        pDevice->destroy(&renderPass);

    for (auto& framebuffer : vFramebuffers)
        pDevice->destroy(&framebuffer);

    vFramebuffers.clear();
    clearImages();

    pDevice = nullptr;
}

void CFramebuffer::create()
{
    createRenderPass();
    createFramebuffer();
}

void CFramebuffer::reCreate()
{
    for (auto& fb : vFramebuffers)
        pDevice->destroy(&fb);

    vFramebuffers.clear();
    clearImages();
    createFramebuffer();
}

void CFramebuffer::begin(vk::CommandBuffer& commandBuffer)
{
    vk::RenderPassBeginInfo renderPassBeginInfo{};
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = getCurrentFramebuffer();
    renderPassBeginInfo.renderArea = renderArea;
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(vClearValues.size());
    renderPassBeginInfo.pClearValues = vClearValues.data();
    commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    vk::Viewport viewport{};
    if (flipViewport)
    {
        viewport.width = static_cast<float>(renderArea.extent.width);
        viewport.height = -static_cast<float>(renderArea.extent.height);
        viewport.x = 0;
        viewport.y = static_cast<float>(renderArea.extent.height);
    }
    else
    {
        viewport.width = renderArea.extent.width;
        viewport.height = renderArea.extent.height;
        viewport.x = 0;
        viewport.y = 0;
    }

    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vk::Rect2D scissor = renderArea;

    commandBuffer.setViewport(0, 1, &viewport);
    commandBuffer.setScissor(0, 1, &scissor);
}

void CFramebuffer::end(vk::CommandBuffer& commandBuffer)
{
    commandBuffer.endRenderPass();
}

void CFramebuffer::addInputReference(uint32_t index, const std::vector<std::string>& vref)
{
    std::vector<vk::AttachmentReference> references;
    for (auto& arg : vref)
    {
        auto attachment = mFbAttachments.find(arg);
        if (attachment != mFbAttachments.end())
        {
            auto& usage = attachment->second.usageFlags;
            vk::ImageLayout imageLayout;
            if (isStorage(usage) && isSampled(usage))
                imageLayout = vk::ImageLayout::eGeneral;
            else
                imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

            references.emplace_back(vk::AttachmentReference{ attachment->second.reference, imageLayout });
        }
            
        else
            log_error("Attachment not found.");
    }
    mInputReferences.emplace(index, references);
}

void CFramebuffer::addOutputReference(uint32_t index, const std::vector<std::string>& vref)
{
    std::vector<vk::AttachmentReference> references;
    for (auto& arg : vref)
    {
        auto attachment = mFbAttachments.find(arg);
        if (attachment != mFbAttachments.end())
        {
            auto& usage = attachment->second.usageFlags;
            vk::ImageLayout imageLayout;
            if (isStorage(usage) && isSampled(usage))
                imageLayout = vk::ImageLayout::eGeneral;
            else
                imageLayout = vk::ImageLayout::eColorAttachmentOptimal;

            references.emplace_back(vk::AttachmentReference{ attachment->second.reference, imageLayout });
        }
        else
            log_error("Attachment not found.");
    }
    mOutputReferences.emplace(index, references);
}

void CFramebuffer::addDescription(uint32_t subpass, const std::string& depthReference)
{
    vk::SubpassDescription description{};
    description.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;

    auto attachmentRef = mOutputReferences.find(subpass);
    if (attachmentRef != mOutputReferences.end())
    {
        description.colorAttachmentCount = static_cast<uint32_t>(attachmentRef->second.size());
        description.pColorAttachments = attachmentRef->second.data();
    }

    auto inputRef = mInputReferences.find(subpass);
    if (inputRef != mInputReferences.end())
    {
        description.inputAttachmentCount = static_cast<uint32_t>(inputRef->second.size());
        description.pInputAttachments = inputRef->second.data();
    }
    description.pDepthStencilAttachment = !depthReference.empty() ? &mDepthReference.at(depthReference) : nullptr;
    vSubpassDesc.emplace_back(description);
}

void CFramebuffer::addSubpassDependency(uint32_t src, uint32_t dst, vk::PipelineStageFlags srcStageMask, vk::PipelineStageFlags dstStageMask, vk::AccessFlags srcAccessMask,
    vk::AccessFlags dstAccessMask, vk::DependencyFlags depFlags)
{
    vk::SubpassDependency dep{};
    dep.srcSubpass = src;
    dep.dstSubpass = dst;
    dep.srcStageMask = srcStageMask;    //Pipeline stage what we waiting
    dep.dstStageMask = dstStageMask;    //Pipeline stage where we waiting
    dep.srcAccessMask = srcAccessMask;  //
    dep.dstAccessMask = dstAccessMask;
    dep.dependencyFlags = depFlags;
    vSubpassDep.emplace_back(dep);
}

void CFramebuffer::setRenderArea(int32_t offset_x, int32_t offset_y, uint32_t width, uint32_t height)
{
    setRenderArea(vk::Offset2D{ offset_x, offset_y }, vk::Extent2D{ width, height });
}

void CFramebuffer::setRenderArea(vk::Offset2D offset, vk::Extent2D extent)
{
    setRenderArea(vk::Rect2D{ offset, extent });
}

void CFramebuffer::setRenderArea(vk::Rect2D&& area)
{
    renderArea = std::move(area);
}

void CFramebuffer::addImage(const std::string& name, vk::Format format, vk::ImageUsageFlags usageFlags, EImageType eImageType, uint32_t layers)
{
    uint32_t reference{ 0 };
    vk::ClearValue clearValue{};
    vk::AttachmentDescription attachmentDescription{};
    attachmentDescription.format = format;
    attachmentDescription.samples = vk::SampleCountFlagBits::e1;
    attachmentDescription.loadOp = vk::AttachmentLoadOp::eClear;
    attachmentDescription.storeOp = vk::AttachmentStoreOp::eDontCare;
    attachmentDescription.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    attachmentDescription.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    attachmentDescription.initialLayout = vk::ImageLayout::eUndefined;

    if (isColorAttachment(usageFlags))
    {
        if (!isInputAttachment(usageFlags))
        {
            if (isTransferSrc(usageFlags))
            {
                attachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
                attachmentDescription.finalLayout = vk::ImageLayout::eTransferSrcOptimal;
            }
            else if (isSampled(usageFlags) && isStorage(usageFlags))
            {
                attachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
                attachmentDescription.finalLayout = vk::ImageLayout::eGeneral;
            }
            else if (isSampled(usageFlags))
            {
                attachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
                attachmentDescription.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            }
            else
            {
                if (format == pDevice->getImageFormat())
                {
                    attachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
                    attachmentDescription.finalLayout = vk::ImageLayout::ePresentSrcKHR;
                }
                else
                    log_error("Cannot use sampled image with input attachment.");
            }
        }
        else
        {
            if (!isSampled(usageFlags))
            {
                attachmentDescription.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
            }
            else
            {
                attachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
                attachmentDescription.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            }
        }
        
        clearValue.setColor(vk::ClearColorValue{ std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f} });
    }
    else if (isDepthAttachment(usageFlags))
    {
        attachmentDescription.loadOp = vk::AttachmentLoadOp::eClear;
        if (isSampled(usageFlags))
        {
            attachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
            attachmentDescription.finalLayout = vk::ImageLayout::eDepthStencilReadOnlyOptimal;
        }
        else
            attachmentDescription.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
        clearValue.setDepthStencil(vk::ClearDepthStencilValue{ 1.0f, 0 });
        mDepthReference.emplace(name, vk::AttachmentReference{ static_cast<uint32_t>(mFbAttachments.size()), vk::ImageLayout::eDepthStencilAttachmentOptimal });
    }

    reference = static_cast<uint32_t>(mFbAttachments.size());
    vAttachDesc.emplace_back(attachmentDescription);
    vClearValues.emplace_back(clearValue);
    mFbAttachments.emplace(name, FFramebufferAttachmentInfo{ format, usageFlags, eImageType, reference, layers });
}

vk::Framebuffer& CFramebuffer::getCurrentFramebuffer()
{
    return getFramebuffer(pDevice->getCurrentFrame());
}

std::unordered_map<std::string, size_t>& CFramebuffer::getCurrentImages()
{
    return getImages(pDevice->getCurrentFrame());
}

void CFramebuffer::createRenderPass()
{
    vk::RenderPassCreateInfo renderPassCI = {};
    renderPassCI.attachmentCount = static_cast<uint32_t>(vAttachDesc.size());
    renderPassCI.pAttachments = vAttachDesc.data();
    renderPassCI.subpassCount = static_cast<uint32_t>(vSubpassDesc.size());
    renderPassCI.pSubpasses = vSubpassDesc.data();
    renderPassCI.dependencyCount = static_cast<uint32_t>(vSubpassDep.size());
    renderPassCI.pDependencies = vSubpassDep.data();
    vk::Result res = pDevice->create(renderPassCI, &renderPass); 
    log_cerror(VkHelper::check(res), "Cannot create render pass.");
}

void CFramebuffer::createFramebuffer()
{
    auto graphics = pDevice->getAPI();

    uint32_t layers = std::numeric_limits<uint32_t>::max();
    auto framesInFlight = pDevice->getFramesInFlight();
    for (size_t frame = 0; frame < framesInFlight; frame++)
    {
        std::vector<vk::ImageView> imageViews{};
        for (auto& [name, attachment] : mFbAttachments)
        {
            auto fullname = name + "_" + std::to_string(frame);
            if (attachment.usageFlags & vk::ImageUsageFlagBits::eDepthStencilAttachment)
            {
                auto depthImage = createImage(attachment, renderArea.extent);
                layers = std::min(layers, depthImage->getLayers());
                imageViews.push_back(depthImage->getDescriptor().imageView);

                depthImageIDX = graphics->addImage(fullname, std::move(depthImage));
                mFramebufferImages[frame].emplace(name, depthImageIDX);
            }
            else
            {
                if (attachment.format == pDevice->getImageFormat())
                {
                    imageViews.push_back(pDevice->getImageViews()[frame]);
                    layers = 1;
                }
                else
                {
                    auto image = createImage(attachment, renderArea.extent);
                    layers = std::min(layers, image->getLayers());
                    imageViews.push_back(image->getDescriptor().imageView);
                    mFramebufferImages[frame].emplace(name, graphics->addImage(fullname, std::move(image)));
                }
            }
        }

        vk::FramebufferCreateInfo framebufferCI = {};
        framebufferCI.pNext = nullptr;
        framebufferCI.renderPass = renderPass;
        framebufferCI.pAttachments = imageViews.data();
        framebufferCI.attachmentCount = static_cast<uint32_t>(imageViews.size());
        framebufferCI.width = renderArea.extent.width;
        framebufferCI.height = renderArea.extent.height;
        framebufferCI.layers = layers;

        vk::Framebuffer framebuffer{ VK_NULL_HANDLE };
        vk::Result res = pDevice->create(framebufferCI, &framebuffer);
        log_cerror(VkHelper::check(res), "Cannot create framebuffer.");
        vFramebuffers.emplace_back(framebuffer);
    }
}

std::unique_ptr<CImage> CFramebuffer::createImage(const FFramebufferAttachmentInfo& attachment, vk::Extent2D extent)
{
    std::unique_ptr<CImage> texture;
    bool translate_layout{ false };

    vk::ImageAspectFlags aspectMask{};
    vk::ImageLayout imageLayout{};

    if (isColorAttachment(attachment.usageFlags))
    {
        aspectMask = vk::ImageAspectFlagBits::eColor;

        if (isTransferSrc(attachment.usageFlags))
            imageLayout = vk::ImageLayout::eTransferSrcOptimal;
        else if (isStorage(attachment.usageFlags))
            imageLayout = vk::ImageLayout::eGeneral;
        else
            imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    }
    else if (isDepthAttachment(attachment.usageFlags))
    {
        aspectMask = vk::ImageAspectFlagBits::eDepth;
        if (isSampled(attachment.usageFlags))
            imageLayout = vk::ImageLayout::eDepthStencilReadOnlyOptimal;
        else
            imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    }
    else
    {
        aspectMask = vk::ImageAspectFlagBits::eColor;
        imageLayout = vk::ImageLayout::eGeneral;
        translate_layout = true;
    }

    switch (attachment.eType)
    {
    case EImageType::e2D: {
        auto tex = std::make_unique<CImage2D>(pDevice); 
        tex->create(extent, attachment.format, imageLayout, attachment.usageFlags, aspectMask, vk::Filter::eNearest, vk::SamplerAddressMode::eClampToEdge, vk::SampleCountFlagBits::e1, translate_layout);
        texture = std::move(tex);
    } break;
    case EImageType::eArray2D: { 
        auto tex = std::make_unique<CImage2DArray>(pDevice);
        tex->create(attachment.layers, extent, attachment.format, imageLayout, attachment.usageFlags, aspectMask, vk::Filter::eLinear, vk::SamplerAddressMode::eClampToEdge, vk::SampleCountFlagBits::e1, translate_layout);
        texture = std::move(tex);
    } break;
    case EImageType::eArrayCube: { 
        auto tex = std::make_unique<CImageCubemapArray>(pDevice);
        tex->create(attachment.layers, extent, attachment.format, imageLayout, attachment.usageFlags, aspectMask, vk::Filter::eLinear, vk::SamplerAddressMode::eClampToEdge, vk::SampleCountFlagBits::e1, translate_layout);
        texture = std::move(tex);
    } break;
    }

    return texture;
}

void CFramebuffer::clearImages()
{
    auto graphics = pDevice->getAPI();

    for (auto& [frame, images] : mFramebufferImages)
    {
        for (auto& [name, image] : images)
            graphics->removeImage(image);
    }

    mFramebufferImages.clear();
}