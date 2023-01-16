#pragma once

#include "EngineStructures.h"

namespace engine
{
	namespace game
	{
		class CSceneLoader
		{
		public:
			static entt::entity load(const std::filesystem::path& scenepath);
			static void save(const entt::entity& root, const std::filesystem::path& scenepath);
		private:
			static entt::entity loadNode(FSceneObjectRaw& object);
			static FSceneObjectRaw saveNode(const entt::entity& node);
		};
	}
}