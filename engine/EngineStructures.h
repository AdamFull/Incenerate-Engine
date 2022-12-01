#pragma once

#include <utility/uparse.hpp>
#include <string>

namespace engine
{
	enum class ERenderApi
	{
		eNone,
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

	struct FWindowCreateInfo
	{
		int32_t width{ 0 };
		int32_t height{ 0 };

		std::string srName;
	};

	void to_json(nlohmann::json& json, const FWindowCreateInfo& type);
	void from_json(const nlohmann::json& json, FWindowCreateInfo& type);

	struct FEngineCreateInfo
	{
		ERenderApi eAPI{ ERenderApi::eNone };
		FWindowCreateInfo window;
		std::string srPipeline;
		std::string srShaders;
	};

	void to_json(nlohmann::json& json, const FEngineCreateInfo& type);
	void from_json(const nlohmann::json& json, FEngineCreateInfo& type);
}