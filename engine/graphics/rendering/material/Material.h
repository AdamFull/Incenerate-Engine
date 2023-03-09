#pragma once

#include "graphics/APIStructures.h"

namespace engine
{
	namespace graphics
	{
		struct FMaterialParameters
		{
			EAlphaMode alphaMode{ EAlphaMode::EOPAQUE };
			float alphaCutoff{ 0.5f };
			glm::vec3 emissiveFactor{ 0.f };
			float normalScale{ 1.f };
			float occlusionStrenth{ 1.f };
			glm::vec4 baseColorFactor{ 1.0f };
			float metallicFactor{ 1.f };
			float roughnessFactor{ 1.f };
			std::vector<std::string> vCompileDefinitions;
			bool doubleSided{ false };

			// Custom extension
			float tessellationFactor{ 0.f };
			float displacementStrength{ 1.f };


			// KHR_materials_clearcoat
			float clearcoatFactor{ 0.f };
			float clearcoatRoughnessFactor{ 0.f };

			// KHR_materials_emissive_strength
			float emissiveStrength{ 1.f };

			// KHR_materials_ior
			float ior{ 1.5f };

			// KHR_materials_iridescence
			float iridescenceFactor{ 0.f };
			float iridescenceIor{ 1.3f };
			float iridescenceThicknessMinimum{ 100.f };
			float iridescenceThicknessMaximum{ 400.f };

			// KHR_materials_sheen
			glm::vec3 sheenColorFactor{ 0.f };
			float sheenRoughnessFactor{ 0.f };

			// KHR_materials_specular
			float specularFactor{ 1.f };
			glm::vec3 specularColorFactor{ 1.f };


			// KHR_materials_transmission
			float transmissionFactor{ 0.f };

			// KHR_materials_unlit ?

			// KHR_materials_volume
			float thicknessFactor{ 0.f };
			float attenuationDistance{INFINITY};
			glm::vec3 attenuationColor{ 1.f };

		};

		void to_json(nlohmann::json& json, const FMaterialParameters& type);
		void from_json(const nlohmann::json& json, FMaterialParameters& type);

		struct FMaterialCreateInfo
		{
			FMaterialParameters parameters;
			std::map<std::string, std::string> textures;
			std::string shader;
		};

		void to_json(nlohmann::json& json, const FMaterialCreateInfo& type);
		void from_json(const nlohmann::json& json, FMaterialCreateInfo& type);

		class CMaterial
		{
		public:
			~CMaterial();

			void setParameters(FMaterialParameters&& mat);
			FMaterialParameters& getParameters() { return parameters; }

			void addTexture(const std::string& srTexture, size_t index = invalid_index);
			const std::unordered_map<std::string, size_t> getTextures() const { return mTextures; }
			const size_t getTexture(const std::string& srTexture) const;

			void setShader(size_t index);
			const size_t getShader() const;

			void incrementUsageCount();
			const size_t getUsageCount() const;
		private:
			FMaterialParameters parameters;
			std::unordered_map<std::string, size_t> mTextures;
			size_t usageCount{ 0 };
			size_t shader_id{ invalid_index };
		};
	}
}