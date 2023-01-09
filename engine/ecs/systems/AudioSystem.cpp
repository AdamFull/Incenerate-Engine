#include "AudioSystem.h"

#include "Engine.h"

#include "ecs/components/AudioComponent.h"
#include "ecs/components/CameraComponent.h"
#include "ecs/components/TransformComponent.h"

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

	{
		auto view = registry.view<FTransformComponent, FCameraComponent>();
		for (auto [entity, transform, camera] : view.each())
		{
			if (camera.active)
			{
				alCall(alListener3f, AL_POSITION, transform.rposition.x, transform.rposition.y, transform.rposition.z);
				float orient[6] = { camera.forward.x, camera.forward.y, camera.forward.z, camera.right.x, camera.right.y, camera.right.z};
				alCall(alListenerfv, AL_ORIENTATION, orient);
			}
		}
	}
	
	{
		auto view = registry.view<FTransformComponent, FAudioComponent>();
		for (auto [entity, transform, audio] : view.each())
		{
			auto& pSource = EGAudio->getSource(audio.asource);

			pSource->setGain(audio.gain);
			pSource->setPosition(transform.rposition);
			pSource->setPitch(audio.pitch);

			if (!audio.shouldStop && !audio.playing)
			{
				pSource->play();
				audio.playing = true;
			}

			if(audio.shouldStop)
			{
				pSource->stop();
				audio.playing = false;
			}

			if (audio.playing)
			{
				auto state = pSource->getState();
				audio.shouldStop = state == EAudioSourceState::eStopped;
			}
		}
	}
}