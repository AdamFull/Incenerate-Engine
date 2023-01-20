#pragma once

#include <utility/uparse.hpp>
#include <utility/uflag.hpp>

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

	enum class ECullingType
	{
		eByPoint,
		eBySphere,
		eByBox
	};

	enum class ERenderStageAvaliableFlagBits
	{
		eShadow = 1 << 0,
		eMesh = 1 << 1,
		eComposition = 1 << 2,
		ePostProcess = 1 << 3
	};

	using ERenderStageAvaliableFlags = utl::flags<ERenderStageAvaliableFlagBits>;


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


	// Scene structures
	struct FPostProcessTonemap
	{
		float gamma{ 2.2f };
		float exposure{ 4.5f };
	};

	void to_json(nlohmann::json& json, const FPostProcessTonemap& type);
	void from_json(const nlohmann::json& json, FPostProcessTonemap& type);

	struct FScenePostProcessing
	{
		FPostProcessTonemap tonemapping;
	};

	void to_json(nlohmann::json& json, const FScenePostProcessing& type);
	void from_json(const nlohmann::json& json, FScenePostProcessing& type);

	struct FSceneObjectRaw
	{
		std::string srName;
		std::map<std::string, nlohmann::json> mComponents;
		std::vector<FSceneObjectRaw> vChildren;
	};

	void to_json(nlohmann::json& json, const FSceneObjectRaw& type);
	void from_json(const nlohmann::json& json, FSceneObjectRaw& type);

	struct FIncenerateScene
	{
		FScenePostProcessing postprocessing;
		FSceneObjectRaw root;
	};

	void to_json(nlohmann::json& json, const FIncenerateScene& type);
	void from_json(const nlohmann::json& json, FIncenerateScene& type);
}

template <>
struct utl::FlagTraits<engine::ERenderStageAvaliableFlagBits>
{
	enum : uint32_t
	{
		allFlags = (uint32_t)engine::ERenderStageAvaliableFlagBits::eShadow | (uint32_t)engine::ERenderStageAvaliableFlagBits::eMesh |
		(uint32_t)engine::ERenderStageAvaliableFlagBits::eComposition | (uint32_t)engine::ERenderStageAvaliableFlagBits::ePostProcess
	};
};