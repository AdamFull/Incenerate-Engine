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
			void __draw(float fDt) override;
		private:
			void audioEdit(ecs::FAudioComponent* object);
			void cameraEdit(ecs::FCameraComponent* object);
			void scriptEdit(ecs::FScriptComponent* object);
			void skyboxEdit(ecs::FEnvironmentComponent* object);
			void sceneEdit(ecs::FSceneComponent* object);
			void rigidbodyEdit(ecs::FTransformComponent* transform, ecs::FRigidBodyComponent* object);
			void particleSystemEdit(ecs::FTransformComponent* transform, ecs::FParticleComponent* object);
			void terrainEdit(ecs::FTerrainComponent* object);
			void meshEdit(ecs::FMeshComponent* object);
		};
	}
}