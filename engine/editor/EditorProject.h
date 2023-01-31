#pragma once

#include "EngineStructures.h"

namespace engine
{
	namespace editor
	{
		struct FIncenerateProject
		{
			std::string assets;
			std::string last_scene;
			FSceneObjectRaw camera;
		};

		void to_json(nlohmann::json& json, const FIncenerateProject& type);
		void from_json(const nlohmann::json& json, FIncenerateProject& type);

		class CEditorProject
		{
		public:
			CEditorProject(entt::entity& editor_camera);
			bool make_new(const std::filesystem::path& path);
			bool open(const std::filesystem::path& path);
			void save();
			void setScenePath(const std::filesystem::path& path);
			bool isProjectOpen();
		private:
			void createOrLoadEditorCamera();
			entt::entity* editor_camera{ nullptr };
			FIncenerateProject project;
			std::filesystem::path projectpath;
		};
	}
}