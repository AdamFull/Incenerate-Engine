#pragma once

namespace engine
{
	namespace game
	{
		class CSceneManager
		{
		public:
			~CSceneManager();

			bool make_new(const std::filesystem::path& path);
			bool load(const std::filesystem::path& path);
			void save();

			const entt::entity& getRoot();
			const std::filesystem::path& getScenePath() const;
		private:
			std::filesystem::path scenepath{};
			entt::entity root{ entt::null };
		};
	}
}