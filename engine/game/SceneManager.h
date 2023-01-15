#pragma once

namespace engine
{
	namespace game
	{
		class CSceneManager
		{
		public:
			~CSceneManager();

			void make_new(const std::filesystem::path& path);
			void load(const std::filesystem::path& path);
			void save();

			const entt::entity& getRoot();
		private:
			std::filesystem::path scenepath{};
			entt::entity root{ entt::null };
		};
	}
}