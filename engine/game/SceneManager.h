#pragma once

namespace engine
{
	namespace game
	{
		class CSceneManager
		{
		public:
			~CSceneManager();

			bool make_new(const std::string& path);
			bool load(const std::string& path);
			void save();
			void release();

			const entt::entity& getRoot();
			const std::string& getScenePath() const;
		private:
			std::string scenepath{};
			entt::entity root{ entt::null };
		};
	}
}