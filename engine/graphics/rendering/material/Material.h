#pragma once

#include "graphics/APIStructures.h"

namespace engine
{
	namespace graphics
	{
		struct FMaterial
		{
			EAlphaMode alphaMode{ EAlphaMode::EOPAQUE };
			float alphaCutoff{ 0.5f };
			glm::vec3 emissiveFactor{ 0.f };
			float emissiveStrength{ 1.f };
			float normalScale{ 1.f };
			float occlusionStrenth{ 1.f };
			glm::vec4 baseColorFactor{ 1.0f };
			float metallicFactor{ 1.f };
			float roughnessFactor{ 1.f };
			float tessellationFactor{ 0.f };
			float tessStrength{ 0.f };
			std::vector<std::string> vCompileDefinitions;
			bool doubleSided{ false };
			bool isSrgb{ false };
		};

		class CMaterial
		{
		public:
			~CMaterial();

			void setParameters(FMaterial&& mat);
			FMaterial& getParameters() { return parameters; }

			void addTexture(const std::string& srTexture, size_t index = invalid_index);
			const std::map<std::string, size_t> getTextures() const { return mTextures; }
			const size_t getTexture(const std::string& srTexture) const;

			void setShader(size_t index);
			const size_t getShader() const;

			void incrementUsageCount();
			const size_t getUsageCount() const;
		private:
			FMaterial parameters;
			std::map<std::string, size_t> mTextures;
			size_t usageCount{ 0 };
			size_t shader_id{ invalid_index };
		};
	}
}