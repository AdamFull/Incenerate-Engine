#include "AudioSystem.h"

#include "Engine.h"

#include "ecs/components/components.h"

using namespace engine::ecs;
using namespace engine::audio;

CAudioSystem::~CAudioSystem()
{
    
}

void CAudioSystem::__create()
{
    
}

void CAudioSystem::__update(float fDt)
{
	auto& registry = EGCoordinator;
	auto editorMode = EGEngine->isEditorMode();
	auto state = EGEngine->getState();

	auto* camera = EGEngine->getActiveCamera();

	if (editorMode && state == EEngineState::eEditing)
	{
		auto ecamera = EGEditor->getCamera();

		if (registry.valid(ecamera))
		{
			auto& transform = registry.get<FTransformComponent>(ecamera);
			auto& camera = registry.get<FCameraComponent>(ecamera);

			updateListener(camera, transform);
		}
	}
	else
	{
		auto view = registry.view<FTransformComponent, FCameraComponent>();
		for (auto [entity, transform, camera] : view.each())
		{
			if (camera.active)
				updateListener(camera, transform);
		}
	}

	
	{
		auto view = registry.view<FTransformComponent, FAudioComponent>();
		for (auto [entity, transform, audio] : view.each())
		{
			auto& pSource = EGAudio->getSource(audio.asource);

			if (pSource)
			{
				pSource->setGain(audio.gain);
				pSource->setPosition(transform.rposition);
				pSource->setPitch(audio.pitch);
			}
		}
	}
}

void CAudioSystem::updateListener(FCameraComponent& camera, FTransformComponent& transform)
{
	if (!isnan(camera.forward.x) || !isnan(camera.forward.y) || !isnan(camera.forward.z))
	{
		alCall(alListener3f, AL_POSITION, transform.rposition.x, transform.rposition.y, transform.rposition.z);
		float orient[6] = { camera.forward.x, camera.forward.y, camera.forward.z, camera.right.x, camera.right.y, camera.right.z };
		alCall(alListenerfv, AL_ORIENTATION, orient);
	}
}