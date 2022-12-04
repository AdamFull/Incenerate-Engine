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

			int32_t albedo{ 0 };
			int32_t metallicRoughness{ 0 };
			int32_t normalMap{ 0 };
			int32_t ambientOcclusion{ 0 };
			int32_t emissionColor{ 0 };
			int32_t heightMap{ 0 };
		};

		class CMaterial
		{
		public:
			void setParameters(FMaterial&& mat);
			const FMaterial& getParameters() { return parameters; }

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