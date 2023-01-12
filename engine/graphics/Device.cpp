#include "Engine.h"
#include "system/window/WindowHandle.h"
#include "buffers/CommandBuffer.h"

#include <set>

#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan_to_string.hpp>

using namespace engine::graphics;
using namespace engine::system::window;

std::vector<const char*> validationLayers{ "VK_LAYER_KHRONOS_validation" };
std::vector<const char*> deviceExtensions{ "VK_KHR_swapchain", "VK_KHR_maintenance4" };

VkResult CDevice::createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL CDevice::validationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    switch (messageSeverity)
    {
    //case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: log_verbose(pCallbackData->pMessage); break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: log_info(pCallbackData->pMessage); break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: log_warning(pCallbackData->pMessage); break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:  log_error(pCallbackData->pMessage); break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT: break;
    }

    return VK_FALSE;
}

void CDevice::destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, callback, pAllocator);
    }
}

void* allocationFunction(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    void* ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

void freeFunction(void* pUserData, void* pMemory)
{
    free(pMemory);
}

void* reallocationFunction(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    return realloc(pOriginal, size);
}

void internalAllocationNotification(void* pUserData, size_t  size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope)
{
}

void internalFreeNotification(void* pUserData, size_t size, VkInternalAllocationType  allocationType, VkSystemAllocationScope allocationScope)
{
}

static vk::AllocationCallbacks* createPAllocator()
{
    vk::AllocationCallbacks* m_allocator = new vk::AllocationCallbacks();
    memset(m_allocator, 0, sizeof(vk::AllocationCallbacks));
    m_allocator->pfnAllocation = (PFN_vkAllocationFunction)(&allocationFunction);
    m_allocator->pfnReallocation = (PFN_vkReallocationFunction)(&reallocationFunction);
    m_allocator->pfnFree = (PFN_vkFreeFunction)&freeFunction;
    m_allocator->pfnInternalAllocation = (PFN_vkInternalAllocationNotification)&internalAllocationNotification;
    m_allocator->pfnInternalFree = (PFN_vkInternalFreeNotification)&internalFreeNotification;
    return m_allocator;
}

CDevice::CDevice(CAPIHandle* api)
{
    pAPI = api;
    pAllocator = createPAllocator();
}

CDevice::~CDevice()
{
    log_debug("Destroying device.");

    cleanupSwapchain();
    destroy(&swapChain);

    destroy(&pipelineCache);
    commandPools.clear();

    vkInstance.destroySurfaceKHR(vkSurface);
    vmaDestroyAllocator(vmaAlloc);
    destroy(&vkDevice);

    if (bValidation)
        destroyDebugUtilsMessengerEXT(vkInstance, vkDebugUtils, (const VkAllocationCallbacks*)pAllocator);

    destroy(&vkInstance);
    free(pAllocator);
    pAllocator = nullptr;
}

void CDevice::create(const FEngineCreateInfo& createInfo)
{
#ifdef _DEBUG
    bValidation = true;
#endif

    log_debug("Validation state: {}", bValidation);
    
    createInstance(createInfo);
    createDebugCallback();
    createSurface();
    createDevice();
    createMemoryAllocator(createInfo);
    createPipelineCache();
    createSwapchain();

    viewportExtent = swapchainExtent;
}

void CDevice::createMemoryAllocator(const FEngineCreateInfo& eci)
{
    VmaVulkanFunctions vk_funcs = {};
    vk_funcs.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
    vk_funcs.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
    vk_funcs.vkAllocateMemory = vkAllocateMemory;
    vk_funcs.vkFreeMemory = vkFreeMemory;
    vk_funcs.vkMapMemory = vkMapMemory;
    vk_funcs.vkUnmapMemory = vkUnmapMemory;
    vk_funcs.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
    vk_funcs.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
    vk_funcs.vkBindBufferMemory = vkBindBufferMemory;
    vk_funcs.vkBindImageMemory = vkBindImageMemory;
    vk_funcs.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
    vk_funcs.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
    vk_funcs.vkCreateBuffer = vkCreateBuffer;
    vk_funcs.vkDestroyBuffer = vkDestroyBuffer;
    vk_funcs.vkCreateImage = vkCreateImage;
    vk_funcs.vkDestroyImage = vkDestroyImage;
    vk_funcs.vkCmdCopyBuffer = vkCmdCopyBuffer;

    VmaAllocatorCreateInfo createInfo{};
    createInfo.instance = vkInstance;
    createInfo.physicalDevice = vkPhysical;
    createInfo.device = vkDevice;
    createInfo.pAllocationCallbacks = (VkAllocationCallbacks*)pAllocator;
    createInfo.vulkanApiVersion = getVulkanVersion(eci.eAPI);
    createInfo.pVulkanFunctions = &vk_funcs;
    
    auto res = (vk::Result)vmaCreateAllocator(&createInfo, &vmaAlloc);
    log_cerror(VkHelper::check(res), "Cannot create vulkan memory allocator.");
}

void CDevice::createInstance(const FEngineCreateInfo& createInfo)
{
    if (bValidation && !VkHelper::checkValidationLayerSupport(validationLayers))
        log_error("Validation layers requested, but not available!");

    auto extensions = VkHelper::getRequiredExtensions(bValidation);

    auto vkVersion = getVulkanVersion(createInfo.eAPI);

    vk::ApplicationInfo appInfo{};
    appInfo.pApplicationName = "vkcore";
    appInfo.applicationVersion = vkVersion;
    appInfo.pEngineName = "voxengine";
    appInfo.engineVersion = vkVersion;
    appInfo.apiVersion = vkVersion;

    log_debug("Creating instance: [name: {}, version {}]", appInfo.pApplicationName, vkVersion);

    vk::InstanceCreateInfo instanceCI{};
    instanceCI.pApplicationInfo = &appInfo;
    instanceCI.enabledLayerCount = 0;
    instanceCI.ppEnabledLayerNames = nullptr;
    instanceCI.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceCI.ppEnabledExtensionNames = extensions.data();

    if (bValidation)
    {
        std::array<vk::ValidationFeatureEnableEXT, 1> validationExt{ vk::ValidationFeatureEnableEXT::eSynchronizationValidation };
        vk::ValidationFeaturesEXT validationFeatures{};
        validationFeatures.enabledValidationFeatureCount = validationExt.size();
        validationFeatures.pEnabledValidationFeatures = validationExt.data();

        instanceCI.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        instanceCI.ppEnabledLayerNames = validationLayers.data();
        instanceCI.pNext = &validationFeatures;
    }

    vk::Result res = create(instanceCI, &vkInstance);
    log_cerror(VkHelper::check(res), "Cannot create vulkan instance.");
}

void CDevice::createDebugCallback()
{
    if (!bValidation)
        return;

    auto createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
        vk::DebugUtilsMessengerCreateFlagsEXT(),
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
        validationCallback,
        nullptr);

    // NOTE: Vulkan-hpp has methods for this, but they trigger linking errors...
    if (createDebugUtilsMessengerEXT(vkInstance, reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&createInfo), (const VkAllocationCallbacks*)pAllocator, &vkDebugUtils) != VK_SUCCESS)
        log_error("failed to set up debug callback!");
}

void CDevice::createSurface()
{
    auto window = CEngine::getInstance()->getWindow()->getWindowPointer();
    log_cerror(vkInstance, "Unable to create surface, cause vulkan instance is not valid");
    VkSurfaceKHR rawSurfaceKHR{ VK_NULL_HANDLE };
    SDL_Vulkan_CreateSurface(window, vkInstance, &rawSurfaceKHR);
    vkSurface = rawSurfaceKHR;
    log_cerror(vkSurface, "Surface creation failed");
}

void CDevice::createDevice()
{
    log_cerror(vkInstance, "Unable to create ligical device, cause vulkan instance is not valid");
    vkPhysical = getPhysicalDevice(deviceExtensions);
    log_cerror(vkPhysical, "No avaliable physical devices. Check device dependencies.");

    auto props = vkPhysical.getProperties();
    log_debug("Selected device: [ID: {}, NAME: {}, VENDOR ID: {}, API: {}]", props.deviceID, props.deviceName, props.vendorID, props.apiVersion);

    if (!isSupportedSampleCount(msaaSamples))
        msaaSamples = vk::SampleCountFlagBits::e1;

    FQueueFamilyIndices indices = findQueueFamilies(vkPhysical);

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamilies)
        queueCreateInfos.push_back({ vk::DeviceQueueCreateFlags(), queueFamily, 1, &queuePriority });

    vk::PhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = true;
    deviceFeatures.sampleRateShading = true;
    deviceFeatures.fillModeNonSolid = true;
    deviceFeatures.multiViewport = true;
    deviceFeatures.geometryShader = true;
    deviceFeatures.tessellationShader = true;
    deviceFeatures.fragmentStoresAndAtomics = true;
    deviceFeatures.logicOp = true;
    deviceFeatures.imageCubeArray = true;
    deviceFeatures.depthClamp = true;
    deviceFeatures.independentBlend = true;

    auto createInfo = vk::DeviceCreateInfo{};
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (bValidation)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }

    vk::Result res = create(createInfo, &vkDevice);
    log_cerror(VkHelper::check(res), "Failed to create logical device.");

    qGraphicsQueue = vkDevice.getQueue(indices.graphicsFamily.value(), 0);
    log_cerror(qGraphicsQueue, "Failed while getting graphics queue.");
    qPresentQueue = vkDevice.getQueue(indices.presentFamily.value(), 0);
    log_cerror(qPresentQueue, "Failed while getting present queue.");
    qComputeQueue = vkDevice.getQueue(indices.computeFamily.value(), 0);
    log_cerror(qComputeQueue, "Failed while getting compute queue.");
    qTransferQueue = vkDevice.getQueue(indices.transferFamily.value(), 0);
    log_cerror(qTransferQueue, "Failed while getting transfer queue.");
}

void CDevice::createPipelineCache()
{
    vk::PipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    vk::Result res = create(pipelineCacheCreateInfo, &pipelineCache);
    log_cerror(VkHelper::check(res), "Cannot create pipeline cache.");
}

void CDevice::createSwapchain()
{
    vk::Result res;
    vk::SwapchainKHR oldSwapchain = swapChain;
    swapChain = VK_NULL_HANDLE;

    auto swapChainSupport = querySwapChainSupport();
    vk::SurfaceFormatKHR surfaceFormat = VkHelper::chooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = VkHelper::chooseSwapPresentMode(swapChainSupport.presentModes);
    swapchainExtent = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = framesInFlight;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        imageCount = swapChainSupport.capabilities.maxImageCount;
    else if (imageCount < swapChainSupport.capabilities.minImageCount)
        imageCount = swapChainSupport.capabilities.minImageCount;

    framesInFlight = imageCount;
    log_debug("Creating swapchain: [Frames in flight {}, extent {}x{}]", framesInFlight, swapchainExtent.width, swapchainExtent.height);

    vk::SwapchainCreateInfoKHR createInfo{};
    createInfo.surface = vkSurface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = swapchainExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;\

    auto indices = findQueueFamilies();
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily)
    {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = oldSwapchain;

    res = create(createInfo, &swapChain);
    log_cerror(VkHelper::check(res), "Swap chain was not created");

    vImages.resize(imageCount);
    res = vkDevice.getSwapchainImagesKHR(swapChain, &imageCount, vImages.data());
    log_cerror(VkHelper::check(res), "Swap chain images was not created");

    imageFormat = surfaceFormat.format;

    if (oldSwapchain)
        destroy(&oldSwapchain);

    //Creating image views
    {
        vImageViews.resize(framesInFlight);
        vk::ImageViewCreateInfo viewInfo{};
        viewInfo.viewType = vk::ImageViewType::e2D;
        viewInfo.format = imageFormat;
        viewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;
        viewInfo.components.r = vk::ComponentSwizzle::eR;
        viewInfo.components.g = vk::ComponentSwizzle::eG;
        viewInfo.components.b = vk::ComponentSwizzle::eB;
        viewInfo.components.a = vk::ComponentSwizzle::eA;

        for (size_t i = 0; i < imageCount; i++)
        {
            viewInfo.image = vImages[i];
            res = create(viewInfo, &vImageViews[i]);
            log_cerror(VkHelper::check(res), "Cannot create image view");
        }
    }

    //Create sync objects
    {
        vImageAvailableSemaphores.resize(imageCount);
        vRenderFinishedSemaphores.resize(imageCount);
        vInFlightFences.resize(imageCount);

        try
        {
            vk::SemaphoreCreateInfo semaphoreCI{};
            vk::FenceCreateInfo fenceCI{};
            fenceCI.flags = vk::FenceCreateFlagBits::eSignaled;
            for (size_t i = 0; i < imageCount; i++)
            {
                res = create(semaphoreCI, &vImageAvailableSemaphores[i]);
                log_cerror(VkHelper::check(res), "Cannot create semaphore");
                res = create(semaphoreCI, &vRenderFinishedSemaphores[i]);
                log_cerror(VkHelper::check(res), "Cannot create semaphore");
                res = create(fenceCI, &vInFlightFences[i]);
                log_cerror(VkHelper::check(res), "Cannot create fence");
            }
        }
        catch (vk::SystemError err)
        {
            log_error("Failed to create synchronization objects for a frame!");
        }
    }
}

void CDevice::cleanupSwapchain()
{
    log_debug("Cleaning up vulkan swapchain.");

    for (size_t i = 0; i < framesInFlight; i++)
    {
        destroy(&vInFlightFences[i]);
        destroy(&vImageViews[i]);
        destroy(&vImageAvailableSemaphores[i]);
        destroy(&vRenderFinishedSemaphores[i]);
    }

    vInFlightFences.clear();
    vImageViews.clear();
    vImageAvailableSemaphores.clear();
    vRenderFinishedSemaphores.clear();
}

void CDevice::updateCommandPools()
{
    for (auto it = commandPools.begin(); it != commandPools.end();)
    {
        if ((*it).second.use_count() <= 1)
        {
            it = commandPools.erase(it);
            continue;
        }
        ++it;
    }
}

void CDevice::recreateSwapchain()
{
    log_debug("Recreating vulkan swapchain.");
    
    commandPools.clear();
    cleanupSwapchain();
    createSwapchain();
    currentFrame = 0;
}

void CDevice::nillViewportFlag()
{
    bViewportRebuild = false;
}

uint32_t CDevice::getVulkanVersion(ERenderApi eAPI)
{
    switch (eAPI)
    {
    case ERenderApi::eVulkan_1_0:
        return VK_API_VERSION_1_0;
    case ERenderApi::eVulkan_1_1:
        return VK_API_VERSION_1_1;
    case ERenderApi::eVulkan_1_2:
        return VK_API_VERSION_1_2;
    case ERenderApi::eVulkan_1_3:
        return VK_API_VERSION_1_3;
    }

    return 0;
}

FQueueFamilyIndices CDevice::findQueueFamilies()
{
    return findQueueFamilies(vkPhysical);
}

FSwapChainSupportDetails CDevice::querySwapChainSupport()
{
    return querySwapChainSupport(vkPhysical);
}

std::vector<vk::SampleCountFlagBits> CDevice::getAvaliableSampleCount()
{
    vk::PhysicalDeviceProperties physicalDeviceProperties;
    vkPhysical.getProperties(&physicalDeviceProperties);
    std::vector<vk::SampleCountFlagBits> avaliableSamples{ vk::SampleCountFlagBits::e1 };

    vk::SampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & vk::SampleCountFlagBits::e64)
        avaliableSamples.push_back(vk::SampleCountFlagBits::e64);
    if (counts & vk::SampleCountFlagBits::e32)
        avaliableSamples.push_back(vk::SampleCountFlagBits::e32);
    if (counts & vk::SampleCountFlagBits::e16)
        avaliableSamples.push_back(vk::SampleCountFlagBits::e16);
    if (counts & vk::SampleCountFlagBits::e8)
        avaliableSamples.push_back(vk::SampleCountFlagBits::e8);
    if (counts & vk::SampleCountFlagBits::e4)
        avaliableSamples.push_back(vk::SampleCountFlagBits::e4);
    if (counts & vk::SampleCountFlagBits::e2)
        avaliableSamples.push_back(vk::SampleCountFlagBits::e2);

    return avaliableSamples;
}

bool CDevice::isSupportedSampleCount(vk::SampleCountFlagBits samples)
{
    auto avaliableSamples = getAvaliableSampleCount();
    auto found = std::find(avaliableSamples.begin(), avaliableSamples.end(), samples);
    return found != avaliableSamples.end();
}

void CDevice::copyOnDeviceBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
    auto cmdBuf = CCommandBuffer(this);
    cmdBuf.create(true, vk::QueueFlagBits::eTransfer);
    auto commandBuffer = cmdBuf.getCommandBuffer();

    vk::BufferCopy copyRegion = {};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    commandBuffer.copyBuffer(srcBuffer, dstBuffer, copyRegion);

    cmdBuf.submitIdle();
}

void CDevice::createImage(vk::Image& image, vk::ImageCreateInfo createInfo, VmaAllocation& allocation)
{
    log_debug("Creating image: [extent {}x{}x{}, type {}, format {}, mips {}, layers {}, samples {}, tiling {}, sharing {}, layout {}]", 
        createInfo.extent.width, createInfo.extent.height, createInfo.extent.depth,
        vk::to_string(createInfo.imageType), vk::to_string(createInfo.format), createInfo.mipLevels, createInfo.arrayLayers,
        vk::to_string(createInfo.samples), vk::to_string(createInfo.tiling), vk::to_string(createInfo.sharingMode), vk::to_string(createInfo.initialLayout));
    log_cerror(vkDevice, "Trying to create image, byt logical device is not valid.");

    VmaAllocationCreateInfo alloc_create_info = {};
    alloc_create_info.usage = VmaMemoryUsage::VMA_MEMORY_USAGE_UNKNOWN; //Device local

    auto ici = (VkImageCreateInfo)createInfo;
    auto img = (VkImage)image;
    auto res = (vk::Result)vmaCreateImage(vmaAlloc, &ici, &alloc_create_info, &img, &allocation, nullptr);
    log_cerror(VkHelper::check(res), "Image was not created");

    image = img;
}

void CDevice::transitionImageLayout(vk::Image& image, std::vector<vk::ImageMemoryBarrier>& vBarriers, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
    auto cmdBuf = CCommandBuffer(this);
    cmdBuf.create(true, vk::QueueFlagBits::eTransfer);
    auto commandBuffer = cmdBuf.getCommandBuffer();

    transitionImageLayout(commandBuffer, image, vBarriers, oldLayout, newLayout);

    cmdBuf.submitIdle();
}

void CDevice::transitionImageLayout(vk::CommandBuffer& internalBuffer, vk::Image& image, std::vector<vk::ImageMemoryBarrier>& vBarriers, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    for (auto& barrier : vBarriers)
    {
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.image = image;

        if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
        {
            barrier.srcAccessMask = (vk::AccessFlagBits)0;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        }
        else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eGeneral)
        {
            barrier.srcAccessMask = (vk::AccessFlagBits)0;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        }
        else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eColorAttachmentOptimal)
        {
            barrier.srcAccessMask = (vk::AccessFlagBits)0;
            barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        }
        else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
        {
            barrier.srcAccessMask = (vk::AccessFlagBits)0;
            barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
        }
        else if (oldLayout == vk::ImageLayout::eColorAttachmentOptimal && newLayout == vk::ImageLayout::eTransferSrcOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

            sourceStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        }
        else if (oldLayout == vk::ImageLayout::eTransferSrcOptimal && newLayout == vk::ImageLayout::eColorAttachmentOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        }
        else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eTransferSrcOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eTransfer;
        }
        else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else if (oldLayout == vk::ImageLayout::eTransferSrcOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            sourceStage = vk::PipelineStageFlagBits::eTransfer;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.srcAccessMask = (vk::AccessFlagBits)0;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
            destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
        }
        else
        {
            log_error("Unsupported layout transition!");
        }
    }

    internalBuffer.pipelineBarrier(
        sourceStage,
        destinationStage,
        vk::DependencyFlags(),
        0, nullptr, 0, nullptr,
        static_cast<uint32_t>(vBarriers.size()), vBarriers.data());
}

void CDevice::copyBufferToImage(vk::Buffer& buffer, vk::Image& image, std::vector<vk::BufferImageCopy> vRegions)
{
    auto cmdBuf = CCommandBuffer(this);
    cmdBuf.create(true, vk::QueueFlagBits::eTransfer);
    auto commandBuffer = cmdBuf.getCommandBuffer();
    commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, static_cast<uint32_t>(vRegions.size()), vRegions.data());
    cmdBuf.submitIdle();
}

void CDevice::copyTo(vk::CommandBuffer& commandBuffer, vk::Image& src, vk::Image& dst, vk::ImageLayout srcLayout, vk::ImageLayout dstLayout, vk::ImageCopy& region)
{
    commandBuffer.copyImage(src, srcLayout, dst, dstLayout, 1, &region);
}

void CDevice::createSampler(vk::Sampler& sampler, vk::Filter magFilter, vk::SamplerAddressMode eAddressMode, bool anisotropy, bool compareOp, uint32_t mipLevels)
{
    log_debug("Creating sampler: [filter {}, addr {}, anisotropy {}, compare {}, mips {}]", 
        vk::to_string(magFilter), vk::to_string(eAddressMode), anisotropy, compareOp, mipLevels);

    log_cerror(vkPhysical, "Trying to create sampler, but physical device is invalid.");
    vk::SamplerCreateInfo samplerInfo{};
    samplerInfo.magFilter = magFilter;
    samplerInfo.minFilter = magFilter;
    samplerInfo.addressModeU = eAddressMode;
    samplerInfo.addressModeV = eAddressMode;
    samplerInfo.addressModeW = eAddressMode;

    vk::PhysicalDeviceProperties properties{};
    properties = vkPhysical.getProperties();

    samplerInfo.anisotropyEnable = static_cast<vk::Bool32>(anisotropy);
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = vk::BorderColor::eIntOpaqueWhite;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    samplerInfo.compareEnable = static_cast<vk::Bool32>(compareOp);
    samplerInfo.compareOp = vk::CompareOp::eLess;

    samplerInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(mipLevels);

    // TODO: Result handle
    vk::Result res = create(samplerInfo, &sampler);
    log_cerror(VkHelper::check(res), "Texture sampler was not created");
}

vk::Format CDevice::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
    log_cerror(vkPhysical, "Trying to find supported format, but physical device is invalid.");
    for (vk::Format format : candidates)
    {
        vk::FormatProperties props;
        vkPhysical.getFormatProperties(format, &props);

        if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
            return format;
        else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
            return format;
    }

    // TODO: Handle null result
    log_error("Failed to find supported format!");
}

std::vector<vk::Format> CDevice::findSupportedFormats(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
    std::vector<vk::Format> vFormats;
    log_cerror(vkPhysical, "Trying to find supported format, but physical device is invalid.");
    for (vk::Format format : candidates)
    {
        vk::FormatProperties props;
        vkPhysical.getFormatProperties(format, &props);

        if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
            vFormats.emplace_back(format);
        else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
            vFormats.emplace_back(format);
    }

    return vFormats;
}

vk::Format CDevice::getDepthFormat()
{
    return findSupportedFormat
    (
        {
            vk::Format::eD32Sfloat,
            vk::Format::eD32SfloatS8Uint,
            vk::Format::eD24UnormS8Uint
        },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}

std::vector<vk::Format> CDevice::getTextureCompressionFormats()
{
    std::vector<vk::Format> vFormats;

    log_cerror(vkPhysical, "Trying to create image, byt logical device is not valid.");
    vk::PhysicalDeviceFeatures supportedFeatures = vkPhysical.getFeatures();

    if (supportedFeatures.textureCompressionBC)
    {
        auto supportedCBC = findSupportedFormats
        (
            {
                vk::Format::eBc1RgbUnormBlock,
                vk::Format::eBc1RgbSrgbBlock,
                vk::Format::eBc1RgbaUnormBlock,
                vk::Format::eBc1RgbaSrgbBlock,
                vk::Format::eBc2UnormBlock,
                vk::Format::eBc2SrgbBlock,
                vk::Format::eBc3UnormBlock,
                vk::Format::eBc3SrgbBlock,
                vk::Format::eBc4UnormBlock,
                vk::Format::eBc4SnormBlock,
                vk::Format::eBc5UnormBlock,
                vk::Format::eBc5SnormBlock,
                vk::Format::eBc6HUfloatBlock,
                vk::Format::eBc6HSfloatBlock,
                vk::Format::eBc7UnormBlock,
                vk::Format::eBc7SrgbBlock
            },
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eSampledImage | vk::FormatFeatureFlagBits::eTransferDst
        );
        vFormats.insert(vFormats.end(), supportedCBC.begin(), supportedCBC.end());
    }

    if (supportedFeatures.textureCompressionASTC_LDR)
    {
        auto supportedCBC = findSupportedFormats
        (
            {
                vk::Format::eAstc4x4UnormBlock,
                vk::Format::eAstc4x4SrgbBlock,
                vk::Format::eAstc5x4UnormBlock,
                vk::Format::eAstc5x4SrgbBlock,
                vk::Format::eAstc5x5UnormBlock,
                vk::Format::eAstc5x5SrgbBlock,
                vk::Format::eAstc6x5UnormBlock,
                vk::Format::eAstc6x5SrgbBlock,
                vk::Format::eAstc6x6UnormBlock,
                vk::Format::eAstc6x6SrgbBlock,
                vk::Format::eAstc8x5UnormBlock,
                vk::Format::eAstc8x5SrgbBlock,
                vk::Format::eAstc8x6UnormBlock,
                vk::Format::eAstc8x6SrgbBlock,
                vk::Format::eAstc8x8UnormBlock,
                vk::Format::eAstc8x8SrgbBlock,
                vk::Format::eAstc10x5UnormBlock,
                vk::Format::eAstc10x5SrgbBlock,
                vk::Format::eAstc10x6UnormBlock,
                vk::Format::eAstc10x6SrgbBlock,
                vk::Format::eAstc10x8UnormBlock,
                vk::Format::eAstc10x8SrgbBlock,
                vk::Format::eAstc10x10UnormBlock,
                vk::Format::eAstc10x10SrgbBlock,
                vk::Format::eAstc12x10UnormBlock,
                vk::Format::eAstc12x10SrgbBlock,
                vk::Format::eAstc12x12UnormBlock,
                vk::Format::eAstc12x12SrgbBlock
            },
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eSampledImage | vk::FormatFeatureFlagBits::eTransferDst
        );
        vFormats.insert(vFormats.end(), supportedCBC.begin(), supportedCBC.end());
    }

    if (supportedFeatures.textureCompressionETC2)
    {
        auto supportedCBC = findSupportedFormats
        (
            {
                vk::Format::eEtc2R8G8B8UnormBlock,
                vk::Format::eEtc2R8G8B8SrgbBlock,
                vk::Format::eEtc2R8G8B8A1UnormBlock,
                vk::Format::eEtc2R8G8B8A1SrgbBlock,
                vk::Format::eEtc2R8G8B8A8UnormBlock,
                vk::Format::eEtc2R8G8B8A8SrgbBlock
            },
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eSampledImage | vk::FormatFeatureFlagBits::eTransferDst
        );
        vFormats.insert(vFormats.end(), supportedCBC.begin(), supportedCBC.end());
    }

    return vFormats;
}

vk::Result CDevice::acquireNextImage(uint32_t* imageIndex)
{
    vk::Result res = vkDevice.waitForFences(1, &vInFlightFences[currentFrame], VK_TRUE, (std::numeric_limits<uint64_t>::max)());
    log_cerror(VkHelper::check(res), "Waiting for fences error.");
    res = vkDevice.acquireNextImageKHR(swapChain, (std::numeric_limits<uint64_t>::max)(), vImageAvailableSemaphores[currentFrame], nullptr, imageIndex);
    return res;
}

vk::Result CDevice::submitCommandBuffers(const vk::CommandBuffer* commandBuffer, uint32_t* imageIndex, vk::QueueFlagBits queueBit)
{
    vk::Result res;
    vk::SubmitInfo submitInfo = {};
    vk::Semaphore waitSemaphores[] = { vImageAvailableSemaphores[currentFrame] };
    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = commandBuffer;

    vk::Semaphore signalSemaphores[] = { vRenderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    res = vkDevice.resetFences(1, &vInFlightFences[currentFrame]);
    log_cerror(VkHelper::check(res), "Cannot reset fences.");

    try
    {
        auto& queue = getQueue(queueBit);
        queue.submit(submitInfo, vInFlightFences[currentFrame]);
    }
    catch (vk::SystemError err)
    {
        log_error("failed to submit draw command buffer!");
    }

    vk::PresentInfoKHR presentInfo = {};
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    vk::SwapchainKHR swapChains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = imageIndex;

    auto& present = qPresentQueue;
    res = present.presentKHR(presentInfo);

    currentFrame = (currentFrame + 1) % framesInFlight;

    return res;
}

float CDevice::getAspect(bool automatic)
{
    auto ext = getExtent(automatic);
    return static_cast<float>(ext.width) / static_cast<float>(ext.height);
}

vk::Extent2D CDevice::getExtent(bool automatic)
{
    if (automatic && EGEngine->isEditorMode())
        return viewportExtent;
    return swapchainExtent;
}

void CDevice::setViewportExtent(vk::Extent2D extent)
{
    if (extent != viewportExtent)
    {
        bViewportRebuild = true;
        viewportExtent = extent;
    }
}

CAPIHandle* CDevice::getAPI()
{
    return pAPI;
}

const std::shared_ptr<CCommandPool>& CDevice::getCommandPool(const std::thread::id& threadId)
{
    auto it = commandPools.find(threadId);
    if (it != commandPools.end())
        return it->second;

    commandPools.emplace(threadId, std::make_shared<CCommandPool>(this));
    commandPools[threadId]->create(threadId);
    return commandPools[threadId];
}

vk::Queue& CDevice::getQueue(vk::QueueFlagBits flag)
{
    switch (flag)
    {
    case vk::QueueFlagBits::eGraphics: {
        return qGraphicsQueue;
    } break;
    case vk::QueueFlagBits::eCompute: {
        return qComputeQueue;
    } break;
    case vk::QueueFlagBits::eTransfer: {
        return qTransferQueue;
    } break;
    }
}

FQueueFamilyIndices CDevice::findQueueFamilies(const vk::PhysicalDevice& device)
{
    FQueueFamilyIndices indices;

    auto queueFamilies = device.getQueueFamilyProperties();

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
        {
            indices.graphicsFamily = i;
        }

        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eCompute)
        {
            indices.computeFamily = i;
        }

        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eTransfer)
        {
            indices.transferFamily = i;
        }

        if (queueFamily.queueCount > 0 && device.getSurfaceSupportKHR(i, vkSurface))
        {
            indices.presentFamily = i;
        }

        if (indices.isComplete())
        {
            break;
        }

        i++;
    }

    return indices;
}

FSwapChainSupportDetails CDevice::querySwapChainSupport(const vk::PhysicalDevice& device)
{
    FSwapChainSupportDetails details;
    details.capabilities = device.getSurfaceCapabilitiesKHR(vkSurface);
    details.formats = device.getSurfaceFormatsKHR(vkSurface);
    details.presentModes = device.getSurfacePresentModesKHR(vkSurface);

    return details;
}

vk::Extent2D CDevice::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)())
        return capabilities.currentExtent;
    else
    {
        vk::Extent2D actualExtent = { static_cast<uint32_t>(CWindowHandle::iWidth), static_cast<uint32_t>(CWindowHandle::iHeight) };

        actualExtent.width = (std::max)(capabilities.minImageExtent.width, (std::min)(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = (std::max)(capabilities.minImageExtent.height, (std::min)(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}

vk::PhysicalDevice CDevice::getPhysicalDevice(const std::vector<const char*>& deviceExtensions)
{
    vk::PhysicalDevice selected_device{nullptr};
    auto devices = getAvaliablePhysicalDevices(deviceExtensions);

    for (auto& device : devices)
    {
        auto props = device.getProperties();

        if (!selected_device && props.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            selected_device = device;
    }

    if (!selected_device)
        selected_device = devices.front();

    if (selected_device && isDeviceSuitable(selected_device, deviceExtensions))
        return selected_device;

    return nullptr;
}

std::vector<vk::PhysicalDevice> CDevice::getAvaliablePhysicalDevices(const std::vector<const char*>& deviceExtensions)
{
    auto devices = vkInstance.enumeratePhysicalDevices();
    std::vector<vk::PhysicalDevice> output_devices;
    if (devices.size() == 0)
        log_error("Failed to find GPUs with Vulkan support!");

    for (const auto& device : devices)
    {
        if (isDeviceSuitable(device, deviceExtensions))
            output_devices.emplace_back(device);
    }

    if (output_devices.size() == 0)
        log_error("Failed to find a suitable GPU!");

    return output_devices;
}

bool CDevice::isDeviceSuitable(const vk::PhysicalDevice& device, const std::vector<const char*>& deviceExtensions)
{
    FQueueFamilyIndices indices = findQueueFamilies(device);

    bool extensionsSupported = VkHelper::checkDeviceExtensionSupport(device, deviceExtensions);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        FSwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    vk::PhysicalDeviceFeatures supportedFeatures = device.getFeatures();

    return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy && supportedFeatures.sampleRateShading;
}