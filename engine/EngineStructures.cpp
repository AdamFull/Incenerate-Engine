#include "EngineStructures.h"

#include "parse/glmparse.h"

namespace engine
{
	NLOHMANN_JSON_SERIALIZE_ENUM
	(
		ERenderApi,
		{
			{ERenderApi::eVulkan_1_0, "vk10"},
			{ERenderApi::eVulkan_1_1, "vk11"},
			{ERenderApi::eVulkan_1_2, "vk12"},
			{ERenderApi::eVulkan_1_3, "vk13"}
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

	NLOHMANN_JSON_SERIALIZE_ENUM
	(
		EPhysicsShapeType,
		{
			{EPhysicsShapeType::eBox, "box"},
			{EPhysicsShapeType::eCapsule, "capsule"},
			{EPhysicsShapeType::eCone, "cone"},
			{EPhysicsShapeType::eCylinder, "cylinder"},
			{EPhysicsShapeType::eSphere, "sphere"},
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

	// Scene structures


	void to_json(nlohmann::json& json, const FSceneObjectRaw& type)
	{
		utl::serialize_from("name", json, type.srName, !type.srName.empty());
		utl::serialize_from("components", json, type.mComponents, !type.mComponents.empty());
		utl::serialize_from("children", json, type.vChildren, !type.vChildren.empty());
	}

	void from_json(const nlohmann::json& json, FSceneObjectRaw& type)
	{
		utl::parse_to("name", json, type.srName);
		utl::parse_to("components", json, type.mComponents);
		utl::parse_to("children", json, type.vChildren);
	}


	void to_json(nlohmann::json& json, const FIncenerateScene& type)
	{
		json = nlohmann::json();
		utl::serialize_from("scene", json, type.root, true);
	}

	void from_json(const nlohmann::json& json, FIncenerateScene& type)
	{
		utl::parse_to("scene", json, type.root);
	}


	// Post process
	void to_json(nlohmann::json& json, const FPostProcessFXAA& type)
	{
		utl::serialize_from("enable", json, type.enable, type.enable != false);
		utl::serialize_from("quality", json, type.quality, type.quality != 0);
		utl::serialize_from("threshold", json, type.threshold, type.threshold != 0);
		utl::serialize_from("threshold_min", json, type.threshold_min, type.threshold_min != 0);
	}

	void from_json(const nlohmann::json& json, FPostProcessFXAA& type)
	{
		utl::parse_to("enable", json, type.enable);
		utl::parse_to("quality", json, type.quality);
		utl::parse_to("threshold", json, type.threshold);
		utl::parse_to("threshold_min", json, type.threshold_min);
	}


	void to_json(nlohmann::json& json, const FPostProcessDOF& type)
	{
		utl::serialize_from("enable", json, type.enable, type.enable != false);
		//utl::serialize_from("blur_scale", json, type.blur_scale, type.blur_scale != 1.f);
		//utl::serialize_from("blur_strength", json, type.blur_strength, type.blur_strength != 1.f);
		utl::serialize_from("bokeh_samples", json, type.bokeh_samples, type.bokeh_samples != 5);
		utl::serialize_from("bokeh_poly", json, type.bokeh_poly, type.bokeh_poly != 8.f);
		utl::serialize_from("focus_point", json, type.focus_point, type.focus_point != 8.24f);
		utl::serialize_from("near_field", json, type.near_field, type.near_field != 0.3f);
		utl::serialize_from("near_transition", json, type.near_transition, type.near_transition != 0.125f);
		utl::serialize_from("far_field", json, type.far_field, type.far_field != 8.f);
		utl::serialize_from("far_transition", json, type.far_transition, type.far_transition != 3.f);
	}

	void from_json(const nlohmann::json& json, FPostProcessDOF& type)
	{
		utl::parse_to("enable", json, type.enable);
		//utl::parse_to("blur_scale", json, type.blur_scale);
		//utl::parse_to("blur_strength", json, type.blur_strength);
		utl::parse_to("bokeh_samples", json, type.bokeh_samples);
		utl::parse_to("bokeh_poly", json, type.bokeh_poly);
		utl::parse_to("focus_point", json, type.focus_point);
		utl::parse_to("near_field", json, type.near_field);
		utl::parse_to("near_transition", json, type.near_transition);
		utl::parse_to("far_field", json, type.far_field);
		utl::parse_to("far_transition", json, type.far_transition);
	}


	void to_json(nlohmann::json& json, const FPostProcessBloom& type)
	{
		utl::serialize_from("enable", json, type.enable, type.enable != false);
		utl::serialize_from("threshold", json, type.threshold, type.threshold != 1.f);
		utl::serialize_from("filter_radius", json, type.filter_radius, type.filter_radius != 0.005f);
		utl::serialize_from("strength", json, type.strength, type.strength != 0.04f);
	}

	void from_json(const nlohmann::json& json, FPostProcessBloom& type)
	{
		utl::parse_to("enable", json, type.enable);
		utl::parse_to("threshold", json, type.threshold);
		utl::parse_to("filter_radius", json, type.filter_radius);
		utl::parse_to("strength", json, type.strength);
	}


	void to_json(nlohmann::json& json, const FPostProcessCA& type)
	{
		utl::serialize_from("enable", json, type.enable, type.enable != false);
		utl::serialize_from("distortion", json, type.distortion, type.distortion != 2.2f);
		utl::serialize_from("iterations", json, type.iterations, type.iterations != 12);
	}

	void from_json(const nlohmann::json& json, FPostProcessCA& type)
	{
		utl::parse_to("enable", json, type.enable);
		utl::parse_to("distortion", json, type.distortion);
		utl::parse_to("iterations", json, type.iterations);
	}


	void to_json(nlohmann::json& json, const FPostProcessVignette& type)
	{
		utl::serialize_from("enable", json, type.enable, type.enable != false);
		utl::serialize_from("inner", json, type.inner, type.inner != 0.15f);
		utl::serialize_from("outer", json, type.outer, type.outer != 1.35f);
		utl::serialize_from("opacity", json, type.opacity, type.opacity != 0.85f);
	}

	void from_json(const nlohmann::json& json, FPostProcessVignette& type)
	{
		utl::parse_to("enable", json, type.enable);
		utl::parse_to("inner", json, type.inner);
		utl::parse_to("outer", json, type.outer);
		utl::parse_to("opacity", json, type.opacity);
	}


	void to_json(nlohmann::json& json, const FPostProcessTonemap& type)
	{
		utl::serialize_from("enable", json, type.enable, type.enable != false);
		utl::serialize_from("adaptive", json, type.adaptive, type.adaptive != false);
		utl::serialize_from("gamma", json, type.gamma, type.gamma != 2.2f);
		utl::serialize_from("exposure", json, type.exposure, type.exposure != 4.5f);
		utl::serialize_from("whitePoint", json, type.whitePoint, type.whitePoint != 4.f);
		utl::serialize_from("lumMin", json, type.lumMin, type.lumMin != -8.f);
		utl::serialize_from("lumMax", json, type.lumMax, type.lumMax != 3.f);
		utl::serialize_from("tau", json, type.tau, type.tau != 1.1f);
	}

	void from_json(const nlohmann::json& json, FPostProcessTonemap& type)
	{
		utl::parse_to("enable", json, type.enable);
		utl::parse_to("adaptive", json, type.adaptive);
		utl::parse_to("gamma", json, type.gamma);
		utl::parse_to("exposure", json, type.exposure);
		utl::parse_to("whitePoint", json, type.whitePoint);
		utl::parse_to("lumMin", json, type.lumMin);
		utl::parse_to("lumMax", json, type.lumMax);
		utl::parse_to("tau", json, type.tau);
	}

	void to_json(nlohmann::json& json, const FPostProcessFilmGrain& type)
	{
		utl::serialize_from("enable", json, type.enable, type.enable != false);
		utl::serialize_from("amount", json, type.amount, type.amount != 0.01f);
	}

	void from_json(const nlohmann::json& json, FPostProcessFilmGrain& type)
	{
		utl::parse_to("enable", json, type.enable);
		utl::parse_to("amount", json, type.amount);
	}

	void to_json(nlohmann::json& json, const FPostProcessFog& type)
	{
		utl::serialize_from("enable", json, type.enable, type.enable != false);
		utl::serialize_from("color", json, type.color, type.color != glm::vec3(1.f));
		utl::serialize_from("density", json, type.density, type.density != 1.f);
	}

	void from_json(const nlohmann::json& json, FPostProcessFog& type)
	{
		utl::parse_to("enable", json, type.enable);
		utl::parse_to("color", json, type.color);
		utl::parse_to("density", json, type.density);
	}


	void to_json(nlohmann::json& json, const FPostProcess& type)
	{
		utl::serialize_from("fxaa", json, type.fxaa, true);
		utl::serialize_from("dof", json, type.dof, true);
		utl::serialize_from("bloom", json, type.bloom, true);
		utl::serialize_from("chromatic_aberration", json, type.aberration, true);
		utl::serialize_from("vignette", json, type.vignette, true);
		utl::serialize_from("filmgrain", json, type.filmgrain, true);
		utl::serialize_from("fog", json, type.fog, true);
		utl::serialize_from("tonemap", json, type.tonemap, true);
	}

	void from_json(const nlohmann::json& json, FPostProcess& type)
	{
		utl::parse_to("fxaa", json, type.fxaa);
		utl::parse_to("dof", json, type.dof);
		utl::parse_to("bloom", json, type.bloom);
		utl::parse_to("chromatic_aberration", json, type.aberration);
		utl::parse_to("vignette", json, type.vignette);
		utl::parse_to("filmgrain", json, type.filmgrain);
		utl::parse_to("fog", json, type.fog);
		utl::parse_to("tonemap", json, type.tonemap);
	}
}