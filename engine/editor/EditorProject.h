#pragma once

#include <utility/uparse.hpp>

namespace engine
{
	namespace editor
	{
		struct FIncenerateProject
		{
			std::string assets;
			std::string default_scene;
		};

		void to_json(nlohmann::json& json, const FIncenerateProject& type);
		void from_json(const nlohmann::json& json, FIncenerateProject& type);

		class CEditorProject
		{
		public:
			bool make_new(const std::filesystem::path& path);
			bool open(const std::filesystem::path& path);
			void save();
		private:
			FIncenerateProject project;
			std::filesystem::path projectpath;
		};
	}
}