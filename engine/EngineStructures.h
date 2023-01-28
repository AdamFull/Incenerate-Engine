#pragma once

#include <utility/uparse.hpp>
#include <utility/uflag.hpp>

namespace engine
{
	const std::array<const char*, 5> fxaa_quality_variant{ "1.0", "0.75", "0.5", "0.25", "0.0" };
	const std::array<const char*, 5> fxaa_threshold_variant{ "0.333", "0.250", "0.166", "0.125", "0.063" };
	const std::array<const char*, 3> fxaa_threshold_min_variant{ "0.0833", "0.0625", "0.0312" };

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
		FSceneObjectRaw root;
	};

	void to_json(nlohmann::json& json, const FIncenerateScene& type);
	void from_json(const nlohmann::json& json, FIncenerateScene& type);

	// Post processing
	struct FPostProcessFXAA
	{
		bool enable{ true };
		int quality{ 0 };
		int threshold{ 0 };
		int threshold_min{ 0 };
	};

	void to_json(nlohmann::json& json, const FPostProcessFXAA& type);
	void from_json(const nlohmann::json& json, FPostProcessFXAA& type);

	struct FPostProcessDOF
	{
		bool enable{ false };
		float blur_scale{ 1.f };
		float blur_strength{ 1.f };
		int bokeh_samples{ 5 };
		float bokeh_poly{ 8.f };
		float focus_point{ 8.24f };
		float near_field{ 0.3f };
		float near_transition{ 0.125f };
		float far_field{ 8.f };
		float far_transition{ 3.f };
	};

	void to_json(nlohmann::json& json, const FPostProcessDOF& type);
	void from_json(const nlohmann::json& json, FPostProcessDOF& type);

	struct FPostProcessBloom
	{
		bool enable{ false };
		float threshold{ 1.f };
		float filter_radius{ 0.005f };
		float strength{ 0.04f };
	};

	void to_json(nlohmann::json& json, const FPostProcessBloom& type);
	void from_json(const nlohmann::json& json, FPostProcessBloom& type);

	struct FPostProcessCA
	{
		bool enable{ false };
		float distortion{ 2.2f };
		int iterations{ 12 };
	};

	void to_json(nlohmann::json& json, const FPostProcessCA& type);
	void from_json(const nlohmann::json& json, FPostProcessCA& type);

	struct FPostProcessVignette
	{
		bool enable{ false };
		float inner{ 0.15f };
		float outer{ 1.35f };
		float opacity{ 0.85f };
	};

	void to_json(nlohmann::json& json, const FPostProcessVignette& type);
	void from_json(const nlohmann::json& json, FPostProcessVignette& type);

	struct FPostProcessTonemap
	{
		bool enable{ true };
		float gamma{ 2.2f };
		float exposure{ 4.5f };
	};

	void to_json(nlohmann::json& json, const FPostProcessTonemap& type);
	void from_json(const nlohmann::json& json, FPostProcessTonemap& type);

	struct FPostProcess
	{
		// fxaa
		FPostProcessFXAA fxaa;
		// DOF
		FPostProcessDOF dof;
		// Bloom
		FPostProcessBloom bloom;
		// Chromatic aberration
		FPostProcessCA aberration;
		// Vignette
		FPostProcessVignette vignette;
		// Tonemapping
		FPostProcessTonemap tonemap;
	};

	void to_json(nlohmann::json& json, const FPostProcess& type);
	void from_json(const nlohmann::json& json, FPostProcess& type);
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