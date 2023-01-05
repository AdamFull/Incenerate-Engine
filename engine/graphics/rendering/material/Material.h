#pragma once

namespace engine
{
	namespace graphics
	{
		struct FMaterial
		{
			enum class EAlphaMode
			{
				EOPAQUE,
				EMASK,
				EBLEND
			};

			EAlphaMode alphaMode{ EAlphaMode::EOPAQUE };
			float alphaCutoff{ 0.5f };
			glm::vec3 emissiveFactor{ 0.f };
			float normalScale{ 1.f };
			float occlusionStrenth{ 1.f };
			glm::vec4 baseColorFactor{ 1.0f };
			float metallicFactor{ 1.f };
			float roughnessFactor{ 1.f };
			float tessellationFactor{ 0.f };
			float tessStrength{ 0.f };
			std::vector<std::string> vCompileDefinitions;
			bool doubleSided{ false };

			size_t albedo{ invalid_index };
			size_t metallicRoughness{ invalid_index };
			size_t normalMap{ invalid_index };
			size_t ambientOcclusion{ invalid_index };
			size_t emissionColor{ invalid_index };
			size_t heightMap{ invalid_index };
		};

		class CMaterial
		{
		public:
			void setParameters(FMaterial&& mat);
			FMaterial& getParameters() { return parameters; }

			void setTexture(const std::string& srTexture, size_t index = invalid_index);
			size_t getTexture(const std::string& srTexture);

			void setShader(size_t index);
			size_t getShader();
		private:
			FMaterial parameters;
			std::map<std::string, size_t> mTextures;
			size_t iShader{ invalid_index };
		};
	}
}