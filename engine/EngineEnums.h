#pragma once

#include <utility/uparse.hpp>

namespace engine
{
	enum class ERenderApi
	{
		eVulkan_1_0,
		eVulkan_1_1,
		eVulkan_1_2,
		eVulkan_1_3
	};

	enum class EImageType
	{
		e2D,
		e3D,
		eCubeMap,
		eArray2D,
		eArray3D,
		eArrayCube
	};
}