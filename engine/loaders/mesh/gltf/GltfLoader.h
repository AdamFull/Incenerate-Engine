#pragma once

#include "external/utility/tiny_gltf.h"

#include "game/SceneGraph.hpp"

#include <filesystem>
#include <vector>


namespace engine
{
	namespace loaders
	{
		class CGltfLoader
		{
		public:
			void load(const std::string& source, const std::unique_ptr<game::CSceneNode>& pRoot);

		private:
			void loadNode(const std::unique_ptr<game::CSceneNode>& pNode, const tinygltf::Node& node, uint32_t nodeIndex, const tinygltf::Model& model, float globalscale);
			void loadMeshComponent(const std::unique_ptr<game::CSceneNode>& pNode, const tinygltf::Node& node, const tinygltf::Model& model);
			void loadCameraComponent(const std::unique_ptr<game::CSceneNode>& pNode, const tinygltf::Node& node, const tinygltf::Model& model);
			void loadLightComponent(const std::unique_ptr<game::CSceneNode>& pNode, uint32_t light_index, const tinygltf::Node& node, const tinygltf::Model& model);

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