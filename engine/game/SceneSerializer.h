#pragma once

#include "EngineStructures.h"

namespace engine
{
	namespace game
	{
		class CSceneLoader
		{
		public:
			static entt::entity load(const std::string& scenepath);
			static void save(const entt::entity& root, const std::string& scenepath);
			static entt::entity loadNode(const std::string& mountpoint, FSceneObjectRaw& object);
			static FSceneObjectRaw saveNode(const entt::entity& node, bool save_all = false);

			static void saveComponents(std::map<std::string, std::any>& components, const entt::entity& node);
			static void applyComponents(const std::map<std::string, std::any>& components, const entt::entity& node);
		};
	}
}