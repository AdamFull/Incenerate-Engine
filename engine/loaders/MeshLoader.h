#pragma once

#include "game/SceneGraph.hpp"

namespace engine
{
	namespace loaders
	{
		class CMeshLoader
		{
		public:
			static void load(const std::string& source, const std::unique_ptr<game::CSceneNode>& pRoot);
		};
	}
}