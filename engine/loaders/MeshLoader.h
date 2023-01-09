#pragma once

namespace engine
{
	namespace loaders
	{
		class CMeshLoader
		{
		public:
			static void load(const std::string& source, const entt::entity& pRoot);
		};
	}
}