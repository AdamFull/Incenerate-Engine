#pragma once

#include <vulkan/vulkan.hpp>

namespace engine
{
	namespace ecs
	{
		struct effectshared
		{
			static size_t createImage(const std::string& name, vk::Format format, bool mips = false);
			static void tryReCreateImage(const std::string& name, size_t& image_id, vk::Format format, bool mips = false);
		};
	}
}