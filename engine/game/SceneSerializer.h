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

		class CSceneLoader
		{
		public:
			static entt::entity load(const std::filesystem::path& scenepath);
			static void save(const entt::entity& root, const std::filesystem::path& scenepath);
		private:
			static void loadNodes(const entt::entity& parent, const std::vector<FSceneObjectRaw>& vObjects);
			static void serializeNodes(const entt::entity& parent, std::vector<FSceneObjectRaw>& vObjects);
		};
	}
}