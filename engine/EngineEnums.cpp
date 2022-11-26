#include "EngineEnums.h"

namespace engine
{
	NLOHMANN_JSON_SERIALIZE_ENUM
	(
		ERenderApi,
		{
			{ERenderApi::eVulkan_1_0, "vk_1_0"},
			{ERenderApi::eVulkan_1_1, "vk_1_1"},
			{ERenderApi::eVulkan_1_2, "vk_1_2"},
			{ERenderApi::eVulkan_1_3, "vk_1_3"}
		}
	);

	NLOHMANN_JSON_SERIALIZE_ENUM
	(
		EImageType,
		{
			{EImageType::e2D, "2d"},
			{EImageType::e3D, "3d"},
			{EImageType::eCubeMap, "cube"},
			{EImageType::eArray2D, "array2d"},
			{EImageType::eArray3D, "array3d"},
			{EImageType::eArrayCube, "arrayCube"}
		}
	);
}