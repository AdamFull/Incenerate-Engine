#pragma once

#include <vma/vk_mem_alloc.hpp>

#include "buffers/CommandPool.h"
#include "QueueFamily.h"
#include "DebugUtils.h"
#include "EngineStructures.h"

#include <mutex>

namespace engine
{
	namespace graphics
	{
        struct FSwapChainSupportDetails
        {
            vk::SurfaceCapabilitiesKHR capabilities;
            std::vector<vk::SurfaceFormatKHR> formats;
            std::vector<vk::PresentModeKHR> presentModes;
        };

		class CDevice
		{
		public:
            CDevice(CAPIHandle* api);
			~CDevice();

			void create(const FEngineCreateInfo& createInfo, IWindowAdapter* window);

			void updateCommandPools();

            void recreateSwapchain(int32_t width, int32_t height);
            void nillViewportFlag();

            inline void GPUWait() { vkDevice.waitIdle(); }

            /***************************************************Helpers***************************************************/
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

            void copyOnDeviceBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size, vk::DeviceSize srcOffset = 0, vk::DeviceSize dstOffset = 0);
            void createImage(vk::Image& image, vk::ImageCreateInfo createInfo, vma::Allocation& allocation, vma::MemoryUsage usage = vma::MemoryUsage::eUnknown);

            void makeMemoryBarrier(vk::CommandBuffer& commandBuffer, const vk::MemoryBarrier2KHR& barrier2KHR);
            void makeImageMemoryBarrier(vk::CommandBuffer& commandBuffer, const vk::ImageMemoryBarrier2KHR& imageBarrier2KHR);

            void transitionImageLayoutTransfer(vk::ImageMemoryBarrier2& barrier);
            void transitionImageLayoutGraphics(vk::ImageMemoryBarrier2& barrier);
            void transitionImageLayoutTransfer(vk::CommandBuffer& commandBuffer, vk::ImageMemoryBarrier2& barrier);
            void transitionImageLayoutGraphics(vk::CommandBuffer& commandBuffer, vk::ImageMemoryBarrier2& barrier);

            void copyBufferToImage(vk::Buffer& buffer, vk::Image& image, std::vector<vk::BufferImageCopy> vRegions);
            void copyBufferToImage(vk::CommandBuffer& commandBuffer, vk::Buffer& buffer, vk::Image& image, std::vector<vk::BufferImageCopy> vRegions);
            void createSampler(vk::Sampler& sampler, vk::Filter magFilter, vk::SamplerAddressMode eAddressMode, bool anisotropy, bool compareOp, uint32_t mipLevels);

            vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
            std::vector<vk::Format> findSupportedFormats(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
            vk::Format getDepthFormat();
            std::vector<vk::Format> getTextureCompressionFormats();

            void takeScreenshot(const std::filesystem::path& filepath);
            void readPixel(size_t id, uint32_t x, uint32_t y, void* pixel);

            /**************************************************Getters********************************************/
            inline vk::Instance& getVkInstance() { return vkInstance; }
            inline vk::SurfaceKHR& getSurface() { return vkSurface; }
            const std::shared_ptr<CCommandPool>& getCommandPool(vk::QueueFlags queueFlags, const std::thread::id& threadId = std::this_thread::get_id());

            inline vk::PhysicalDevice& getPhysical() { return vkPhysical; }
            inline vk::Device& getLogical() { return vkDevice; }
            inline vk::Queue& getGraphicsQueue() { return qGraphicsQueue; }
            inline vk::Queue& getPresentQueue() { return qPresentQueue; }
            inline vk::Queue& getComputeQueue() { return qComputeQueue; }
            inline vk::Queue& getTransferQueue() { return qTransferQueue; }
            vk::Queue& getQueue(vk::QueueFlags flags);

            inline vk::SampleCountFlagBits getSamples() { return msaaSamples; }
            inline vk::AllocationCallbacks* getAllocator() { return pAllocator; }

            inline vma::Allocator& getVMAAllocator() { return vmaAlloc; }

            const vk::PipelineCache& getPipelineCache() const { return pipelineCache; }

            uint32_t getQueueFamilyIndex(uint32_t type);

            void getOccupedDeviceMemory(vk::MemoryHeapFlags memoryFlags, size_t& totalMemory, size_t& usedMemory);

            bool isValidationEnabled() const { return bValidation; }

            const std::unique_ptr<CDebugUtils>& getDebugUtils();

            /**************************************************Swapchain********************************************/
            vk::Result acquireNextImage(uint32_t* imageIndex);
            vk::Result submitCommandBuffers(const vk::CommandBuffer* commandBuffer, uint32_t* imageIndex, vk::QueueFlags ququeFlags);

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
            inline vk::Result create(vk::RenderPassCreateInfo2& info, vk::RenderPass* ref)
            {
                return vkDevice.createRenderPass2(&info, pAllocator, ref);
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
            template <>
            inline vk::Result create(vk::QueryPoolCreateInfo& info, vk::QueryPool* ref)
            {
                return vkDevice.createQueryPool(&info, pAllocator, ref);
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
            template <>
            inline void destroy(vk::QueryPool* ref) { vkDevice.destroyQueryPool(*ref, pAllocator); }

        private:
            void createMemoryAllocator(const FEngineCreateInfo& createInfo);
            void createInstance(const FEngineCreateInfo& createInfo, IWindowAdapter* window);
            void createSurface(IWindowAdapter* window);
            void selectPhysicalDeviceAndExtensions();
            void createDevice();
            void createPipelineCache();
            void createSwapchain(int32_t width, int32_t height);

            void cleanupSwapchain();

            FSwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device);
            vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& surfaceCapabilities, int32_t width, int32_t height);

        private:
            uint32_t vkVersion{ 0u };

            CAPIHandle* m_pAPI{ nullptr };
            vk::DynamicLoader dl;
            vk::Instance vkInstance{ VK_NULL_HANDLE }; // Main vulkan instance
            vk::SurfaceKHR vkSurface{ VK_NULL_HANDLE }; // Vulkan's drawing surface
            std::map<vk::QueueFlags, std::map<std::thread::id, std::shared_ptr<CCommandPool>>> commandPools;
            vk::AllocationCallbacks* pAllocator{ nullptr };
            vma::Allocator vmaAlloc{ VK_NULL_HANDLE };

            std::unique_ptr<CDebugUtils> pDebugUtils;

            std::mutex cplock;

            CQueueFamilies queueFamilies;

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
            uint32_t framesInFlight{ 4 };
            uint32_t currentFrame{ 0 };

            bool bEditorMode{ false };
		};
	}
}