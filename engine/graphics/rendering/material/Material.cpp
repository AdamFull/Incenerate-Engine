#include "Material.h"

#include "parse/glmparse.h"
#include "Engine.h"

namespace engine
{
	namespace graphics
	{
		void to_json(nlohmann::json& json, const FMaterialParameters& type)
		{
			utl::serialize_from("alpha_mode", json, type.alphaMode, type.alphaMode != EAlphaMode::EOPAQUE);
			utl::serialize_from("alpha_cutoff", json, type.alphaCutoff, type.alphaCutoff != 0.5f);
			utl::serialize_from("emissive_factor", json, type.emissiveFactor, type.emissiveFactor != glm::vec3(0.f));
			utl::serialize_from("normal_scale", json, type.normalScale, type.normalScale != 1.f);
			utl::serialize_from("occlusion_strenth", json, type.occlusionStrenth, type.occlusionStrenth != 1.f);
			utl::serialize_from("base_color_factor", json, type.baseColorFactor, type.baseColorFactor != glm::vec4(1.f));
			utl::serialize_from("metallic_factor", json, type.metallicFactor, type.metallicFactor != 1.f);
			utl::serialize_from("roughness_factor", json, type.roughnessFactor, type.roughnessFactor != 1.f);
			utl::serialize_from("compile_definitions", json, type.vCompileDefinitions, !type.vCompileDefinitions.empty());
			utl::serialize_from("double_sided", json, type.doubleSided, type.doubleSided);

			// Custom extension
			utl::serialize_from("tessellation_factor", json, type.tessellationFactor, type.tessellationFactor != 0.f);
			utl::serialize_from("displacement_strength", json, type.displacementStrength, type.displacementStrength != 1.f);

			// KHR_materials_clearcoat
			utl::serialize_from("clearcoat_factor", json, type.clearcoatFactor, type.clearcoatFactor != 0.f);
			utl::serialize_from("clearcoat_roughness_factor", json, type.clearcoatRoughnessFactor, type.clearcoatRoughnessFactor != 0.f);

			// KHR_materials_emissive_strength
			utl::serialize_from("emissive_strength", json, type.emissiveStrength, type.emissiveStrength != 1.f);

			// KHR_materials_ior
			utl::serialize_from("ior", json, type.ior, type.ior != 1.5f);

			// KHR_materials_iridescence
			utl::serialize_from("iridescence_factor", json, type.iridescenceFactor, type.iridescenceFactor != 0.f);
			utl::serialize_from("iridescence_ior", json, type.iridescenceIor, type.iridescenceIor != 1.3f);
			utl::serialize_from("iridescence_thickness_minimum", json, type.iridescenceThicknessMinimum, type.iridescenceThicknessMinimum != 100.f);
			utl::serialize_from("iridescence_thickness_maximum", json, type.iridescenceThicknessMaximum, type.iridescenceThicknessMaximum != 400.f);

			// KHR_materials_sheen
			utl::serialize_from("sheen_color_factor", json, type.sheenColorFactor, type.sheenColorFactor != glm::vec3(0.f));
			utl::serialize_from("sheen_roughness_factor", json, type.sheenRoughnessFactor, type.sheenRoughnessFactor != 0.f);

			// KHR_materials_specular
			utl::serialize_from("specular_factor", json, type.specularFactor, type.specularFactor != 1.f);
			utl::serialize_from("specular_color_factor", json, type.specularColorFactor, type.specularColorFactor != glm::vec3(1.f));

			// KHR_materials_transmission
			utl::serialize_from("transmission_factor", json, type.transmissionFactor, type.transmissionFactor != 0.f);

			// KHR_materials_unlit ?

			// KHR_materials_volume
			utl::serialize_from("thickness_factor", json, type.thicknessFactor, type.thicknessFactor != 0.f);
			utl::serialize_from("attenuation_distance", json, type.attenuationDistance, type.attenuationDistance != INFINITY);
			utl::serialize_from("attenuation_color", json, type.attenuationColor, type.attenuationColor != glm::vec3(1.f));
		}

		void from_json(const nlohmann::json& json, FMaterialParameters& type)
		{
			utl::parse_to("alpha_mode", json, type.alphaMode);
			utl::parse_to("alpha_cutoff", json, type.alphaCutoff);
			utl::parse_to("emissive_factor", json, type.emissiveFactor);
			utl::parse_to("normal_scale", json, type.normalScale);
			utl::parse_to("occlusion_strenth", json, type.occlusionStrenth);
			utl::parse_to("base_color_factor", json, type.baseColorFactor);
			utl::parse_to("metallic_factor", json, type.metallicFactor);
			utl::parse_to("roughness_factor", json, type.roughnessFactor);
			utl::parse_to("compile_definitions", json, type.vCompileDefinitions);
			utl::parse_to("double_sided", json, type.doubleSided);

			// Custom extension
			utl::parse_to("tessellation_factor", json, type.tessellationFactor);
			utl::parse_to("displacement_strength", json, type.displacementStrength);

			// KHR_materials_clearcoat
			utl::parse_to("clearcoat_factor", json, type.clearcoatFactor);
			utl::parse_to("clearcoat_roughness_factor", json, type.clearcoatRoughnessFactor);

			// KHR_materials_emissive_strength
			utl::parse_to("emissive_strength", json, type.emissiveStrength);

			// KHR_materials_ior
			utl::parse_to("ior", json, type.ior);

			// KHR_materials_iridescence
			utl::parse_to("iridescence_factor", json, type.iridescenceFactor);
			utl::parse_to("iridescence_ior", json, type.iridescenceIor);
			utl::parse_to("iridescence_thickness_minimum", json, type.iridescenceThicknessMinimum);
			utl::parse_to("iridescence_thickness_maximum", json, type.iridescenceThicknessMaximum);

			// KHR_materials_sheen
			utl::parse_to("sheen_color_factor", json, type.sheenColorFactor);
			utl::parse_to("sheen_roughness_factor", json, type.sheenRoughnessFactor);

			// KHR_materials_specular
			utl::parse_to("specular_factor", json, type.specularFactor);
			utl::parse_to("specular_color_factor", json, type.specularColorFactor);

			// KHR_materials_transmission
			utl::parse_to("transmission_factor", json, type.transmissionFactor);

			// KHR_materials_unlit ?

			// KHR_materials_volume
			utl::parse_to("thickness_factor", json, type.thicknessFactor);
			utl::parse_to("attenuation_distance", json, type.attenuationDistance);
			utl::parse_to("attenuation_color", json, type.attenuationColor);
		}


		void to_json(nlohmann::json& json, const FMaterialCreateInfo& type)
		{
			utl::serialize_from("parameters", json, type.parameters, true);
			utl::serialize_from("textures", json, type.textures, !type.textures.empty());
			utl::serialize_from("shader", json, type.shader, !type.shader.empty());
		}

		void from_json(const nlohmann::json& json, FMaterialCreateInfo& type)
		{
			utl::parse_to("parameters", json, type.parameters);
			utl::parse_to("textures", json, type.textures);
			utl::parse_to("shader", json, type.shader);
		}
	}
}

using namespace engine::graphics;

CMaterial::~CMaterial()
{
	auto& graphics = EGEngine->getGraphics();
	graphics->removeShader(shader_id);

	for (auto& [name, id] : mTextures)
		graphics->removeImage(id);
}

void CMaterial::setParameters(FMaterialParameters&& mat)
{
	parameters = std::move(mat);
}

void CMaterial::addTexture(const std::string& srTexture, size_t index)
{
	mTextures[srTexture] = index;
}

const size_t CMaterial::getTexture(const std::string& srTexture) const
{
	if (auto found = mTextures.find(srTexture); found != mTextures.end())
		return found->second;

	return invalid_index;
}

void CMaterial::setShader(size_t index)
{
	shader_id = index;
}

const size_t CMaterial::getShader() const
{
	return shader_id;
}

void CMaterial::incrementUsageCount()
{
	usageCount++;
}

const size_t CMaterial::getUsageCount() const
{
	return usageCount;
}