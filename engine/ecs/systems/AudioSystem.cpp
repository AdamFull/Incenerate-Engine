#include "AudioSystem.h"

#include "Engine.h"

#include "ecs/components/AudioComponent.h"
#include "ecs/components/CameraComponent.h"

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
	{
		auto view = EGCoordinator.view<FCameraComponent>();
		for (auto [entity, camera] : view.each())
		{
			if (camera.active)
			{
				auto& pNode = EGSceneGraph->find(entity, true);
				auto transform = pNode->getTransform();

				alCall(alListener3f, AL_POSITION, transform.position.x, transform.position.y, transform.position.z);
				float orient[6] = { camera.forward.x, camera.forward.y, camera.forward.z, camera.right.x, camera.right.y, camera.right.z};
				alCall(alListenerfv, AL_ORIENTATION, orient);
			}
		}
	}
	
	{
		auto view = EGCoordinator.view<FAudioComponent>();
		for (auto [entity, audio] : view.each())
		{
			auto& pNode = EGSceneGraph->find(entity, true);
			auto transform = pNode->getTransform();

			auto& pSource = EGAudio->getSource(audio.asource);

			pSource->setGain(audio.gain);
			pSource->setPosition(transform.position);
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