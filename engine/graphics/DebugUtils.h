#pragma once

#include <vulkan/vulkan.hpp>

namespace engine
{
	namespace graphics
	{
		class CDebugUtils
		{
		public:
			CDebugUtils() = default;
			CDebugUtils(CDevice* device);
			~CDebugUtils();

			void initialize();

			void beginLabel(vk::CommandBuffer& commandBuffer, const char* label);
			void beginLabel(vk::CommandBuffer& commandBuffer, const char* label, const glm::vec4& color);
			void insertLabel(vk::CommandBuffer& commandBuffer, const char* label);
			void insertLabel(vk::CommandBuffer& commandBuffer, const char* label, const glm::vec4& color);
			void endLabel(vk::CommandBuffer& commandBuffer);

			void queueBeginLabel(vk::Queue& queue, const char* label);
			void queueBeginLabel(vk::Queue& queue, const char* label, const glm::vec4& color);
			void queueInsertLabel(vk::Queue& queue, const char* label);
			void queueInsertLabel(vk::Queue& queue, const char* label, const glm::vec4& color);
			void queueEndLabel(vk::Queue& queue);

			void setObjectName(vk::ObjectType type, uint64_t handle, const char* name);
			void setObjectTag(vk::ObjectType type, uint64_t handle, uint64_t name, const void* data, size_t size);
		protected:
			static VKAPI_ATTR VkBool32 VKAPI_CALL validationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT*, void*);
		private:
			CDevice* pDevice{ nullptr };

			vk::DebugUtilsMessengerEXT vkDebugUtils{ VK_NULL_HANDLE };

			bool bValidation{ false };
			bool bDebugUtils{ false };
			bool bDebugMarker{ false };
		};
	}
}