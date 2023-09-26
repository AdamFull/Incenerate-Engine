#include "DebugUtils.h"

#include "Device.h"
#include "APICompatibility.h"

#include "Helpers.h"

using namespace engine;
using namespace engine::graphics;

void makeLabel(vk::DebugUtilsLabelEXT& labelInfo, const char* label, const glm::vec4& color)
{
	labelInfo.pLabelName = label;
	labelInfo.color[0] = color.x;
	labelInfo.color[1] = color.y;
	labelInfo.color[2] = color.z;
	labelInfo.color[3] = color.w;
}

CDebugUtils::CDebugUtils(CDevice* device)
{
	pDevice = device;
}

CDebugUtils::~CDebugUtils()
{
	auto& vkInstance = pDevice->getVkInstance();
	auto* pAllocator = pDevice->getAllocator();

	if(bValidation)
		vkInstance.destroyDebugUtilsMessengerEXT(vkDebugUtils, pAllocator);
}

void CDebugUtils::initialize()
{
	auto& vkInstance = pDevice->getVkInstance();
	auto* pAllocator = pDevice->getAllocator();
	bValidation = pDevice->isValidationEnabled();

	bDebugUtils = bValidation && APICompatibility::checkInstanceExtensionEnabled(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	bDebugMarker = bValidation && APICompatibility::checkInstanceExtensionEnabled(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);

	if (bValidation)
	{
		auto createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
			vk::DebugUtilsMessengerCreateFlagsEXT(),
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
			validationCallback,
			nullptr);

		vkDebugUtils = vkInstance.createDebugUtilsMessengerEXT(createInfo, pAllocator);
		if (!vkDebugUtils)
		{
			bDebugUtils = false;
			log_error("failed to set up debug callback!");
		}
	}
}

void CDebugUtils::beginLabel(vk::CommandBuffer& commandBuffer, const char* label)
{
	if (!bDebugUtils)
		return;

	auto dbgcolor = encodeIdToColor(utl::const_hash(label));
	beginLabel(commandBuffer, label, dbgcolor);
}

void CDebugUtils::beginLabel(vk::CommandBuffer& commandBuffer, const char* label, const glm::vec4& color)
{
	if (!bDebugUtils)
		return;

	vk::DebugUtilsLabelEXT labelInfo{};
	makeLabel(labelInfo, label, color);

	commandBuffer.beginDebugUtilsLabelEXT(labelInfo);
}

void CDebugUtils::insertLabel(vk::CommandBuffer& commandBuffer, const char* label)
{
	if (!bDebugUtils)
		return;

	auto dbgcolor = encodeIdToColor(utl::const_hash(label));
	insertLabel(commandBuffer, label, dbgcolor);
}

void CDebugUtils::insertLabel(vk::CommandBuffer& commandBuffer, const char* label, const glm::vec4& color)
{
	if (!bDebugUtils)
		return;

	vk::DebugUtilsLabelEXT labelInfo{};
	makeLabel(labelInfo, label, color);

	commandBuffer.insertDebugUtilsLabelEXT(labelInfo);
}

void CDebugUtils::endLabel(vk::CommandBuffer& commandBuffer)
{
	if (!bDebugUtils)
		return;

	commandBuffer.endDebugUtilsLabelEXT();
}

void CDebugUtils::queueBeginLabel(vk::Queue& queue, const char* label)
{
	if (!bDebugUtils)
		return;

	auto dbgcolor = encodeIdToColor(utl::const_hash(label));
	queueBeginLabel(queue, label, dbgcolor);
}

void CDebugUtils::queueInsertLabel(vk::Queue& queue, const char* label)
{
	if (!bDebugUtils)
		return;

	auto dbgcolor = encodeIdToColor(utl::const_hash(label));
	queueBeginLabel(queue, label, dbgcolor);
}

void CDebugUtils::queueBeginLabel(vk::Queue& queue, const char* label, const glm::vec4& color)
{
	if (!bDebugUtils)
		return;

	vk::DebugUtilsLabelEXT labelInfo{};
	makeLabel(labelInfo, label, color);

	queue.beginDebugUtilsLabelEXT(labelInfo);
}

void CDebugUtils::queueInsertLabel(vk::Queue& queue, const char* label, const glm::vec4& color)
{
	if (!bDebugUtils)
		return;

	vk::DebugUtilsLabelEXT labelInfo{};
	makeLabel(labelInfo, label, color);

	queue.insertDebugUtilsLabelEXT(labelInfo);
}

void CDebugUtils::queueEndLabel(vk::Queue& queue)
{
	if (!bDebugUtils)
		return;

	queue.endDebugUtilsLabelEXT();
}

void CDebugUtils::setObjectName(vk::ObjectType type, uint64_t handle, const char* name)
{
	if (!bDebugUtils)
		return;

	auto& vkDevice = pDevice->getLogical();

	vk::DebugUtilsObjectNameInfoEXT nameInfo{};
	nameInfo.objectType = type;
	nameInfo.objectHandle = handle;
	nameInfo.pObjectName = name;

	vkDevice.setDebugUtilsObjectNameEXT(nameInfo);
}

void CDebugUtils::setObjectTag(vk::ObjectType type, uint64_t handle, uint64_t name, const void* data, size_t size)
{
	if (!bDebugUtils)
		return;

	auto& vkDevice = pDevice->getLogical();

	vk::DebugUtilsObjectTagInfoEXT tagInfo{};
	tagInfo.objectType = type;
	tagInfo.objectHandle = handle;
	tagInfo.tagName = name;
	tagInfo.pTag = data;
	tagInfo.tagSize = size;

	vkDevice.setDebugUtilsObjectTagEXT(tagInfo);
}

VKAPI_ATTR VkBool32 VKAPI_CALL CDebugUtils::validationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	switch (messageSeverity)
	{
		//case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: log_verbose(pCallbackData->pMessage); break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: log_info(pCallbackData->pMessage); break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: log_warning(pCallbackData->pMessage); break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:  log_error(pCallbackData->pMessage); std::cout << pCallbackData->pMessage << std::endl; break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT: break;
	}

	return VK_FALSE;
}