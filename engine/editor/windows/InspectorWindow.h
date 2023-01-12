#pragma once

#include "WindowBase.h"
#include "ecs/components/fwd.h"

namespace engine
{
	namespace editor
	{
		class CEditorInspector : public IEditorWindow
		{
		public:
			CEditorInspector(const std::string& sname) { name = sname; }
			virtual ~CEditorInspector() override = default;

			void create() override;
			void __draw() override;
		private:
			void audioEdit(ecs::FAudioComponent* object);
			void audioRemove(ecs::FAudioComponent* object);

			void cameraEdit(ecs::FCameraComponent* object);

			void scriptEdit(ecs::FScriptComponent* object);
			void scriptRemove(ecs::FScriptComponent* object);

			void skyboxEdit(ecs::FSkyboxComponent* object);
			void skyboxRemove(ecs::FSkyboxComponent* object);

			void sceneEdit(ecs::FSceneComponent* object);
			void sceneRemove(ecs::FSceneComponent* object);
		};
	}
}