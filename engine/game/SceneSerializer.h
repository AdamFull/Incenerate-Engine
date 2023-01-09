#pragma once

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
			static entt::entity load(const std::string& scenepath);
			static void save(const entt::entity& root, const std::string& scenepath);
		private:
			static void loadNodes(const entt::entity& parent, const std::vector<FSceneObjectRaw>& vObjects);
			static void serializeNodes(const entt::entity& parent, std::vector<FSceneObjectRaw>& vObjects);
		};
	}
}