#pragma once

#include "EngineEnums.h"

#include <string>

namespace engine
{
	struct FWindowCreateInfo
	{
		int32_t width;
		int32_t height;

		std::string srName;
	};

	struct FEngineCreateInfo
	{
		ERenderApi eAPI;
		FWindowCreateInfo window;
	};
}