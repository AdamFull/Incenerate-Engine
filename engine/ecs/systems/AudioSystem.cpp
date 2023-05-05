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
	auto* camera = EGEngine->getActiveCamera();

	auto view = registry->view<FTransformComponent, FCameraComponent>();
	for (auto [entity, transform, camera] : view.each())
	{
		if (camera.active)
			updateListener(camera, transform);
	}
	
	{
		auto view = registry->view<FTransformComponent, FAudioComponent>();
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
		EGAudio->setListenerPosition(transform.rposition, camera.forward, camera.right);
}