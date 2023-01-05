#pragma once

#include "SceneGraph.hpp"
#include <utility/uparse.hpp>

namespace engine
{
	namespace game
	{
		struct FSceneObjectRaw
		{
			std::string srName;
			std::map<std::string, nlohmann::json> mComponents;
			std::vector<FSceneObjectRaw> vChildren;
		};

		void to_json(nlohmann::json& json, const FSceneObjectRaw& type);
		void from_json(const nlohmann::json& json, FSceneObjectRaw& type);

		struct FGltfSceneRaw
		{
			std::string source;
		};

		void to_json(nlohmann::json& json, const FGltfSceneRaw& type);
		void from_json(const nlohmann::json& json, FGltfSceneRaw& type);

		class CSceneLoader
		{
		public:
			static std::unique_ptr<CSceneNode> load(const std::string& scenepath);
			static void save(const std::unique_ptr<CSceneNode>& pRoot, const std::string& scenepath);
		private:
			static void loadNodes(const std::unique_ptr<CSceneNode>& pParent, const std::vector<FSceneObjectRaw>& vObjects);
			static void serializeNodes(const std::unique_ptr<CSceneNode>& pParent, std::vector<FSceneObjectRaw>& vObjects);
		};
	}
}