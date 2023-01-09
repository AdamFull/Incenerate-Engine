#pragma once

#include "external/utility/tiny_gltf.h"

namespace engine
{
	namespace loaders
	{
		class CGltfLoader
		{
		public:
			void load(const std::string& source, const entt::entity& pRoot);

		private:
			void loadNode(const entt::entity& entity, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model, float globalscale);
			void loadMeshComponent(const entt::entity& entity, const tinygltf::Node& node, const tinygltf::Model& model);
			void loadCameraComponent(const entt::entity& entity, const tinygltf::Node& node, const tinygltf::Model& model);
			void loadLightComponent(const entt::entity& entity, uint32_t light_index, const tinygltf::Node& node, const tinygltf::Model& model);

			void loadMaterials(const tinygltf::Model& model);
			void loadTextures(const tinygltf::Model& model);
		private:
			std::vector<size_t> vTextures;
			std::vector<size_t> vMaterials;

			size_t vbo_id{ invalid_index };
			uint32_t current_primitive{ 0 };
			std::filesystem::path fsParentPath{ "" };
		};
	}
}