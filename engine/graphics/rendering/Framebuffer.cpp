#include "Framebuffer.h"

#include "Device.h"
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
    mFramebufferImages.clear();
    vFramebufferDepth.clear();

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
    mFramebufferImages.clear();
    vFramebufferDepth.clear();
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
        if (isSampled(usageFlags) && !isInputAttachment(usageFlags))
        {
            attachmentDescription.storeOp = vk::AttachmentStoreOp::eStore;
            attachmentDescription.finalLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        }
        else if (isInputAttachment(usageFlags) && !isSampled(usageFlags))
        {
            attachmentDescription.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;
        }
        else if (isInputAttachment(usageFlags) && isSampled(usageFlags))
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

std::unordered_map<std::string, std::shared_ptr<CImage>>& CFramebuffer::getCurrentImages()
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
    auto framesInFlight = pDevice->getFramesInFlight();
    for (size_t frame = 0; frame < framesInFlight; frame++)
    {
        std::vector<vk::ImageView> imageViews{};
        for (auto& [name, attachment] : mFbAttachments)
        {
            if (attachment.usageFlags & vk::ImageUsageFlagBits::eDepthStencilAttachment)
            {
                vFramebufferDepth.emplace_back(createImage(attachment, renderArea.extent));
                mFramebufferImages[frame].emplace(name, vFramebufferDepth.back());
                imageViews.push_back(vFramebufferDepth.back()->getDescriptor().imageView);
            }
            else
            {
                if (attachment.format == pDevice->getImageFormat())
                {
                    imageViews.push_back(pDevice->getImageViews()[frame]);
                }
                else
                {
                    auto image = createImage(attachment, renderArea.extent);
                    mFramebufferImages[frame].emplace(name, image);
                    imageViews.push_back(image->getDescriptor().imageView);
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
        framebufferCI.layers = 1;

        vk::Framebuffer framebuffer{ VK_NULL_HANDLE };
        vk::Result res = pDevice->create(framebufferCI, &framebuffer);
        log_cerror(VkHelper::check(res), "Cannot create framebuffer.");
        vFramebuffers.emplace_back(framebuffer);
    }
}

std::shared_ptr<CImage> CFramebuffer::createImage(const FFramebufferAttachmentInfo& attachment, vk::Extent2D extent)
{
    std::shared_ptr<CImage> texture;
    bool translate_layout{ false };

    vk::ImageAspectFlags aspectMask{};
    vk::ImageLayout imageLayout{};

    if (isColorAttachment(attachment.usageFlags))
    {
        aspectMask = vk::ImageAspectFlagBits::eColor;
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
        auto tex = std::make_shared<CImage2D>(pDevice); 
        tex->create(extent, attachment.format, imageLayout, attachment.usageFlags, aspectMask, vk::Filter::eNearest, vk::SamplerAddressMode::eRepeat, vk::SampleCountFlagBits::e1, translate_layout);
        texture = tex;
    } break;
    case EImageType::eArray2D: { 
        auto tex = std::make_shared<CImage2DArray>(pDevice); 
        tex->create(attachment.layers, extent, attachment.format, imageLayout, attachment.usageFlags, aspectMask, vk::Filter::eLinear, vk::SamplerAddressMode::eClampToEdge, vk::SampleCountFlagBits::e1, translate_layout);
        texture = tex;
    } break;
    case EImageType::eArrayCube: { 
        auto tex = std::make_shared<CImageCubemapArray>(pDevice); 
        tex->create(attachment.layers, extent, attachment.format, imageLayout, attachment.usageFlags, aspectMask, vk::Filter::eLinear, vk::SamplerAddressMode::eClampToEdge, vk::SampleCountFlagBits::e1, translate_layout);
        texture = tex;
    } break;
    }

    return texture;
}