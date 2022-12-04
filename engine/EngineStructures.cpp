#include "EngineStructures.h"

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

	NLOHMANN_JSON_SERIALIZE_ENUM
	(
		ECullingType,
		{
			{ECullingType::eByPoint, "point"},
			{ECullingType::eBySphere, "sphere"},
			{ECullingType::eByBox, "box"}
		}
	);

	NLOHMANN_JSON_SERIALIZE_ENUM
	(
		ERenderStageAvaliableFlagBits,
		{
			{ERenderStageAvaliableFlagBits::eShadow, "shadow"},
			{ERenderStageAvaliableFlagBits::eMesh, "mesh"},
			{ERenderStageAvaliableFlagBits::eComposition, "composition"},
			{ERenderStageAvaliableFlagBits::ePostProcess, "post_process"}
		}
	);


	void to_json(nlohmann::json& json, const FWindowCreateInfo& type)
	{
		json = nlohmann::json
		{
			{"width", type.width},
			{"height", type.height},
			{"name", type.srName}
		};
	}
		
	void from_json(const nlohmann::json & json, FWindowCreateInfo & type)
	{
		utl::parse_to("width", json, type.width);
		utl::parse_to("height", json, type.height);
		utl::parse_to("name", json, type.srName);
	}


	void to_json(nlohmann::json& json, const FEngineCreateInfo& type)
	{
		json = nlohmann::json
		{
			{"api", type.eAPI},
			{"window", type.window},
			{"pipeline", type.srPipeline},
			{"shaders", type.srShaders}
		};
	}

	void from_json(const nlohmann::json & json, FEngineCreateInfo & type)
	{
		utl::parse_to("api", json, type.eAPI);
		utl::parse_to("window", json, type.window);
		utl::parse_to("pipeline", json, type.srPipeline);
		utl::parse_to("shaders", json, type.srShaders);
	}
}