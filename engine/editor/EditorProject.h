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
			bool make_new(const std::string& path);
			bool open(const std::string& path);
			void save();
			void setScenePath(const std::string& path);
			bool isProjectOpen();
		private:
			void createOrLoadEditorCamera();
			entt::entity* editor_camera{ nullptr };
			FIncenerateProject project;
			std::string projectpath;
		};
	}
}