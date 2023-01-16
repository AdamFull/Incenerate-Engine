#pragma once

#include <utility/tiny_gltf.h>
#include <utility/upattern.hpp>

namespace engine
{
	namespace loaders
	{
		// glTF extensions
		namespace ext
		{
			using namespace utl;
			// Ratified Khronos Extensions for glTF 2.0
			constexpr const uint32_t KHR_draco_mesh_compression = "KHR_draco_mesh_compression"_utl_hash;
			constexpr const uint32_t KHR_lights_punctual = "KHR_lights_punctual"_utl_hash;
			constexpr const uint32_t KHR_materials_clearcoat = "KHR_materials_clearcoat"_utl_hash;
			constexpr const uint32_t KHR_materials_emissive_strength = "KHR_materials_emissive_strength"_utl_hash;
			constexpr const uint32_t KHR_materials_ior = "KHR_materials_ior"_utl_hash;
			constexpr const uint32_t KHR_materials_iridescence = "KHR_materials_iridescence"_utl_hash;
			constexpr const uint32_t KHR_materials_sheen = "KHR_materials_sheen"_utl_hash;
			constexpr const uint32_t KHR_materials_specular = "KHR_materials_specular"_utl_hash;
			constexpr const uint32_t KHR_materials_transmission = "KHR_materials_transmission"_utl_hash;
			constexpr const uint32_t KHR_materials_unlit = "KHR_materials_unlit"_utl_hash;
			constexpr const uint32_t KHR_materials_variants = "KHR_materials_variants"_utl_hash;
			constexpr const uint32_t KHR_materials_volume = "KHR_materials_volume"_utl_hash;
			constexpr const uint32_t KHR_mesh_quantization = "KHR_mesh_quantization"_utl_hash;
			constexpr const uint32_t KHR_texture_basisu = "KHR_texture_basisu"_utl_hash;
			constexpr const uint32_t KHR_texture_transform = "KHR_texture_transform"_utl_hash;
			constexpr const uint32_t KHR_xmp_json_ld = "KHR_xmp_json_ld"_utl_hash;

			// Multi-Vendor Extensions for glTF 2.0
			constexpr const uint32_t EXT_lights_image_based = "EXT_lights_image_based"_utl_hash;
			constexpr const uint32_t EXT_mesh_gpu_instancing = "EXT_mesh_gpu_instancing"_utl_hash;
			constexpr const uint32_t EXT_meshopt_compression = "EXT_meshopt_compression"_utl_hash;
			constexpr const uint32_t EXT_texture_webp = "EXT_texture_webp"_utl_hash;

			// Vendor Extensions for glTF 2.0
			constexpr const uint32_t ADOBE_materials_clearcoat_specular = "ADOBE_materials_clearcoat_specular"_utl_hash;
			constexpr const uint32_t ADOBE_materials_thin_transparency = "ADOBE_materials_thin_transparency"_utl_hash;
			constexpr const uint32_t AGI_articulations = "AGI_articulations"_utl_hash;
			constexpr const uint32_t AGI_stk_metadata = "AGI_stk_metadata"_utl_hash;
			constexpr const uint32_t CESIUM_primitive_outline = "CESIUM_primitive_outline"_utl_hash;
			constexpr const uint32_t FB_geometry_metadata = "FB_geometry_metadata"_utl_hash;
			constexpr const uint32_t MSFT_lod = "MSFT_lod"_utl_hash;
			constexpr const uint32_t MSFT_packing_normalRoughnessMetallic = "MSFT_packing_normalRoughnessMetallic"_utl_hash;
			constexpr const uint32_t MSFT_packing_occlusionRoughnessMetallic = "MSFT_packing_occlusionRoughnessMetallic"_utl_hash;
			constexpr const uint32_t MSFT_texture_dds = "MSFT_texture_dds"_utl_hash;

			// Archived Extensions for glTF 2.0
			constexpr const uint32_t KHR_materials_pbrSpecularGlossiness = "KHR_materials_pbrSpecularGlossiness"_utl_hash;
			constexpr const uint32_t KHR_techniques_webgl = "KHR_techniques_webgl"_utl_hash;
			constexpr const uint32_t KHR_xmp = "KHR_xmp"_utl_hash;

			// In-progress Khronos and multi-vendor extensions for glTF 2.0 (n/a now)
			constexpr const uint32_t KHR_animation_pointer = "KHR_animation_pointer"_utl_hash;
			constexpr const uint32_t KHR_audio = "KHR_audio"_utl_hash;
			constexpr const uint32_t KHR_materials_anisotropy = "KHR_materials_anisotropy"_utl_hash;
			constexpr const uint32_t KHR_materials_diffuse_transmission = "KHR_materials_diffuse_transmission"_utl_hash;
			constexpr const uint32_t KHR_materials_sss = "KHR_materials_sss"_utl_hash;

			// Khronos extensions for glTF 1.0
			constexpr const uint32_t KHR_binary_glTF = "KHR_binary_glTF"_utl_hash;
			constexpr const uint32_t KHR_materials_common = "KHR_materials_common"_utl_hash;

			// Vendor extensions for glTF 1.0
			constexpr const uint32_t CESIUM_RTC = "CESIUM_RTC"_utl_hash;
			constexpr const uint32_t WEB3D_quantized_attributes = "WEB3D_quantized_attributes"_utl_hash;
		}

		class CGltfLoader
		{
		public:
			void load(const std::filesystem::path& source, const entt::entity& pRoot);

		private:
			void loadNode(const entt::entity& entity, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model, float globalscale);
			void loadMeshComponent(const entt::entity& entity, const tinygltf::Node& node, const tinygltf::Model& model);
			void loadCameraComponent(const entt::entity& entity, const tinygltf::Node& node, const tinygltf::Model& model);
			void loadLightComponent(const entt::entity& entity, uint32_t light_index, const tinygltf::Node& node, const tinygltf::Model& model);

			void loadMaterials(const tinygltf::Model& model);
			void loadTextures(const tinygltf::Model& model);
			size_t loadTexture(const std::pair<std::filesystem::path, bool>& texpair, vk::Format override);
		private:
			std::vector<std::pair<std::filesystem::path, bool>> vTextures;
			std::vector<size_t> vMaterials;

			size_t vbo_id{ invalid_index };
			uint32_t current_primitive{ 0 };
			std::filesystem::path fsParentPath{ "" };
		};
	}
}