#pragma once

#include <vma/vk_mem_alloc.hpp>

#include "Helpers.h"
#include "buffers/CommandPool.h"

#include <utility/logger/logger.h>

namespace engine
{
	namespace graphics
	{
		class CDevice
		{
		public:
            CDevice(CAPIHandle* api);
			~CDevice();

			void create(const FEngineCreateInfo& createInfo);

			static VKAPI_ATTR VkBool32 VKAPI_CALL validationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT*, void*);

			void updateCommandPools();

            void recreateSwapchain();
            void nillViewportFlag();

            inline void GPUWait() { vkDevice.waitIdle(); }

            uint32_t getVulkanVersion(ERenderApi pAPI);
            /***************************************************Helpers***************************************************/
            FQueueFamilyIndices findQueueFamilies();
            FSwapChainSupportDetails querySwapChainSupport();
            std::vector<vk::SampleCountFlagBits> getAvaliableSampleCount();
            bool isSupportedSampleCount(vk::SampleCountFlagBits samples);

            /*************************************Data transfer***************************************************/
            template <class T>
            void moveToMemory(T raw_data, vk::DeviceMemory& memory, vk::DeviceSize size)
            {
                void* indata = vkDevice->mapMemory(memory, 0, size);
                memcpy(indata, raw_data, static_cast<size_t>(size));
                vkDevice->unmapMemory(memory);
            }

            void copyOnDeviceBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);
            void createImage(vk::Image& image, vk::ImageCreateInfo createInfo, vma::Allocation& allocation);
            void transitionImageLayout(vk::Image& image, std::vector<vk::ImageMemoryBarrier>& vBarriers, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
            void transitionImageLayout(vk::CommandBuffer& internalBuffer, vk::Image& image, std::vector<vk::ImageMemoryBarrier>& vBarriers, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
            void copyBufferToImage(vk::Buffer& buffer, vk::Image& image, std::vector<vk::BufferImageCopy> vRegions);
            void copyTo(vk::CommandBuffer& commandBuffer, vk::Image& src, vk::Image& dst, vk::ImageLayout srcLayout, vk::ImageLayout dstLayout, vk::ImageCopy& region);
            void createSampler(vk::Sampler& sampler, vk::Filter magFilter, vk::SamplerAddressMode eAddressMode, bool anisotropy, bool compareOp, uint32_t mipLevels);

            vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
            std::vector<vk::Format> findSupportedFormats(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
            vk::Format getDepthFormat();
            std::vector<vk::Format> getTextureCompressionFormats();

            /**************************************************Getters********************************************/
            inline vk::Instance& getVkInstance() { return vkInstance; }
            inline vk::SurfaceKHR& getSurface() { return vkSurface; }
            const std::shared_ptr<CCommandPool>& getCommandPool(const std::thread::id& threadId = std::this_thread::get_id());

            inline vk::PhysicalDevice& getPhysical() { return vkPhysical; }
            inline vk::Device& getLogical() { return vkDevice; }
            inline vk::Queue& getGraphicsQueue() { return qGraphicsQueue; }
            inline vk::Queue& getPresentQueue() { return qPresentQueue; }
            inline vk::Queue& getComputeQueue() { return qComputeQueue; }
            inline vk::Queue& getTransferQueue() { return qTransferQueue; }
            vk::Queue& getQueue(vk::QueueFlagBits flag);

            inline vk::SampleCountFlagBits getSamples() { return msaaSamples; }
            inline vk::AllocationCallbacks* getAllocator() { return pAllocator; }

            inline vma::Allocator& getVMAAllocator() { return vmaAlloc; }

            const vk::PipelineCache& getPipelineCache() const { return pipelineCache; }

            /**************************************************Swapchain********************************************/
            vk::Result acquireNextImage(uint32_t* imageIndex);
            vk::Result submitCommandBuffers(const vk::CommandBuffer* commandBuffer, uint32_t* imageIndex, vk::QueueFlagBits queueBit);

            inline std::vector<vk::ImageView>& getImageViews() { return vImageViews; }
            inline vk::Format getImageFormat() { return imageFormat; };
            float getAspect(bool automatic = false);
            vk::Extent2D getExtent(bool automatic = false);
            uint32_t getFramesInFlight() { return framesInFlight; }
            uint32_t getCurrentFrame() { return currentFrame; }

            const bool isViewportResized() const { return bViewportRebuild; }

            void setViewportExtent(vk::Extent2D extent);

            CAPIHandle* getAPI();

            template <class _Ty, class _Ret>
            inline vk::Result create(_Ty& info, _Ret* ref)
            {
                log_error("Unknown object type.");
                return vk::Result::eSuccess;
            }
            template <>
            inline vk::Result create(vk::InstanceCreateInfo& info, vk::Instance* ref)
            {
                return vk::createInstance(&info, pAllocator, ref);
            }
            template <>
            inline vk::Result create(vk::DeviceCreateInfo& info, vk::Device* ref)
            {
                return vkPhysical.createDevice(&info, pAllocator, ref);
            }
            template <>
            inline vk::Result create(vk::SwapchainCreateInfoKHR& info, vk::SwapchainKHR* ref)
            {
                return vkDevice.createSwapchainKHR(&info, pAllocator, ref);
            }
            template <>
            inline vk::Result create(vk::RenderPassCreateInfo& info, vk::RenderPass* ref)
            {
                return vkDevice.createRenderPass(&info, pAllocator, ref);
            }
            template <>
            inline vk::Result create(vk::FramebufferCreateInfo& info, vk::Framebuffer* ref)
            {
                return vkDevice.createFramebuffer(&info, pAllocator, ref);
            }
            template <>
            inline vk::Result create(vk::SemaphoreCreateInfo& info, vk::Semaphore* ref)
            {
                return vkDevice.createSemaphore(&info, pAllocator, ref);
            }
            template <>
            inline vk::Result create(vk::FenceCreateInfo& info, vk::Fence* ref)
            {
                return vkDevice.createFence(&info, pAllocator, ref);
            }
            template <>
            inline vk::Result create(vk::ShaderModuleCreateInfo& info, vk::ShaderModule* ref)
            {
                return vkDevice.createShaderModule(&info, pAllocator, ref);
            }
            template <>
            inline vk::Result create(vk::PipelineLayoutCreateInfo& info, vk::PipelineLayout* ref)
            {
                return vkDevice.createPipelineLayout(&info, pAllocator, ref);
            }
            template <>
            inline vk::Result create(vk::GraphicsPipelineCreateInfo& info, vk::Pipeline* ref)
            {
                return vkDevice.createGraphicsPipelines(pipelineCache, 1, &info, pAllocator, ref);
            }
            template <>
            inline vk::Result create(vk::ComputePipelineCreateInfo& info, vk::Pipeline* ref)
            {
                return vkDevice.createComputePipelines(pipelineCache, 1, &info, pAllocator, ref);
            }
            template <>
            inline vk::Result create(vk::RayTracingPipelineCreateInfoKHR& info, vk::Pipeline* ref)
            {
                return vkDevice.createRayTracingPipelinesKHR(nullptr, pipelineCache, 1, &info, pAllocator, ref);
            }
            template <>
            inline vk::Result create(vk::DescriptorSetLayoutCreateInfo& info, vk::DescriptorSetLayout* ref)
            {
                return vkDevice.createDescriptorSetLayout(&info, pAllocator, ref);
            }
            template <>
            inline vk::Result create(vk::MemoryAllocateInfo& info, vk::DeviceMemory* ref)
            {
                return vkDevice.allocateMemory(&info, pAllocator, ref);
            }
            template <>
            inline vk::Result create(vk::ImageCreateInfo& info, vk::Image* ref)
            {
                return vkDevice.createImage(&info, pAllocator, ref);
            }
            template <>
            inline vk::Result create(vk::ImageViewCreateInfo& info, vk::ImageView* ref)
            {
                return vkDevice.createImageView(&info, pAllocator, ref);
            }
            template <>
            inline vk::Result create(vk::SamplerCreateInfo& info, vk::Sampler* ref)
            {
                return vkDevice.createSampler(&info, pAllocator, ref);
            }
            template <>
            inline vk::Result create(vk::CommandPoolCreateInfo& info, vk::CommandPool* ref)
            {
                return vkDevice.createCommandPool(&info, pAllocator, ref);
            }
            template <>
            inline vk::Result create(vk::CommandBufferAllocateInfo& info, vk::CommandBuffer* ref)
            {
                return vkDevice.allocateCommandBuffers(&info, ref);
            }
            template <>
            inline vk::Result create(vk::DescriptorSetAllocateInfo& info, vk::DescriptorSet* ref)
            {
                return vkDevice.allocateDescriptorSets(&info, ref);
            }
            template <>
            inline vk::Result create(vk::PipelineCacheCreateInfo& info, vk::PipelineCache* ref)
            {
                return vkDevice.createPipelineCache(&info, pAllocator, ref);
            }
            template <>
            inline vk::Result create(vk::BufferCreateInfo& info, vk::Buffer* ref)
            {
                return vkDevice.createBuffer(&info, pAllocator, ref);
            }
            template <>
            inline vk::Result create(vk::DescriptorPoolCreateInfo& info, vk::DescriptorPool* ref)
            {
                return vkDevice.createDescriptorPool(&info, pAllocator, ref);
            }

            template <class _Ty>
            inline void destroy(_Ty* ref) { log_error("Unknown object type."); }
            template <>
            inline void destroy(vk::Sampler* ref) { vkDevice.destroySampler(*ref, pAllocator); }
            template <>
            inline void destroy(vk::Image* ref) { vkDevice.destroyImage(*ref, pAllocator); }
            template <>
            inline void destroy(vk::ImageView* ref) { vkDevice.destroyImageView(*ref, pAllocator); }
            template <>
            inline void destroy(vk::Buffer* ref) { vkDevice.destroyBuffer(*ref, pAllocator); }
            template <>
            inline void destroy(vk::DeviceMemory* ref) { vkDevice.freeMemory(*ref, pAllocator); }
            template <>
            inline void destroy(vk::Framebuffer* ref) { vkDevice.destroyFramebuffer(*ref, pAllocator); }
            template <>
            inline void destroy(vk::RenderPass* ref) { vkDevice.destroyRenderPass(*ref, pAllocator); }
            template <>
            inline void destroy(vk::SwapchainKHR* ref) { vkDevice.destroySwapchainKHR(*ref, pAllocator); }
            template <>
            inline void destroy(vk::Pipeline* ref) { vkDevice.destroyPipeline(*ref, pAllocator); }
            template <>
            inline void destroy(vk::PipelineLayout* ref) { vkDevice.destroyPipelineLayout(*ref, pAllocator); }
            template <>
            inline void destroy(vk::PipelineCache* ref) { vkDevice.destroyPipelineCache(*ref, pAllocator); }
            template <>
            inline void destroy(vk::ShaderModule* ref) { vkDevice.destroyShaderModule(*ref, pAllocator); }
            template <>
            inline void destroy(vk::CommandPool* ref) { vkDevice.destroyCommandPool(*ref, pAllocator); }
            template <>
            inline void destroy(vk::SurfaceKHR* ref) { vkInstance.destroySurfaceKHR(*ref, pAllocator); }
            template <>
            inline void destroy(vk::DescriptorSetLayout* ref) { vkDevice.destroyDescriptorSetLayout(*ref, pAllocator); }
            template <>
            inline void destroy(vk::DescriptorPool* ref) { vkDevice.destroyDescriptorPool(*ref, pAllocator); }
            template <>
            inline void destroy(vk::Semaphore* ref) { vkDevice.destroySemaphore(*ref, pAllocator); }
            template <>
            inline void destroy(vk::Fence* ref) { vkDevice.destroyFence(*ref, pAllocator); }

        private:
            void createMemoryAllocator(const FEngineCreateInfo& createInfo);
            void createInstance(const FEngineCreateInfo& createInfo);
            void createDebugCallback();
            void createSurface();
            void createDevice();
            void createPipelineCache();
            void createSwapchain();

            void cleanupSwapchain();

            FQueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device);
            FSwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device);
            vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& surfaceCapabilities);

            vk::PhysicalDevice getPhysicalDevice(const std::vector<const char*>& deviceExtensions);
            std::vector<vk::PhysicalDevice> getAvaliablePhysicalDevices(const std::vector<const char*>& deviceExtensions);
            bool isDeviceSuitable(const vk::PhysicalDevice& device, const std::vector<const char*>& deviceExtensions);

        private:
            CAPIHandle* pAPI{ nullptr };
            vk::DynamicLoader dl;
            vk::Instance vkInstance{ VK_NULL_HANDLE }; // Main vulkan instance
            vk::DebugUtilsMessengerEXT vkDebugUtils{ VK_NULL_HANDLE };
            vk::SurfaceKHR vkSurface{ VK_NULL_HANDLE }; // Vulkan's drawing surface
            std::map<std::thread::id, std::shared_ptr<CCommandPool>> commandPools;
            vk::AllocationCallbacks* pAllocator{ nullptr };
            vma::Allocator vmaAlloc{ VK_NULL_HANDLE };

            vk::PhysicalDevice vkPhysical;
            vk::Device vkDevice;
            vk::Queue qGraphicsQueue;
            vk::Queue qPresentQueue;
            vk::Queue qComputeQueue;
            vk::Queue qTransferQueue;

            vk::PipelineCache pipelineCache{ VK_NULL_HANDLE };
            vk::SampleCountFlagBits msaaSamples{ vk::SampleCountFlagBits::e1 };
            bool bValidation{ false };

            //Swapchain
            vk::Format imageFormat;
            vk::Extent2D swapchainExtent, viewportExtent;
            vk::SwapchainKHR swapChain{ VK_NULL_HANDLE };
            bool bViewportRebuild{ false };

            std::vector<vk::Image> vImages;
            std::vector<vk::ImageView> vImageViews;
            std::vector<vk::Semaphore> vImageAvailableSemaphores;
            std::vector<vk::Semaphore> vRenderFinishedSemaphores;
            std::vector<vk::Fence> vInFlightFences;
            uint32_t framesInFlight{ 2 };
            uint32_t currentFrame{ 0 };
		};
	}
}