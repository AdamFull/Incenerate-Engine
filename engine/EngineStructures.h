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
}

template <>
struct utl::FlagTraits<engine::ERenderStageAvaliableFlagBits>
{
	static VULKAN_HPP_CONST_OR_CONSTEXPR bool isBitmask = true;

	enum : uint32_t
	{
		allFlags = (uint32_t)engine::ERenderStageAvaliableFlagBits::eShadow | (uint32_t)engine::ERenderStageAvaliableFlagBits::eMesh |
		(uint32_t)engine::ERenderStageAvaliableFlagBits::eComposition | (uint32_t)engine::ERenderStageAvaliableFlagBits::ePostProcess
	};
};