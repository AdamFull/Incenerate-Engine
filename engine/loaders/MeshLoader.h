#pragma once

namespace engine
{
	namespace loaders
	{
		class CMeshLoader
		{
		public:
			static void load(const std::filesystem::path& source, const entt::entity& pRoot);
		};
	}
}