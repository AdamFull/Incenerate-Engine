#include "AudioSystem.h"

using namespace engine::audio;

CAudioSystem::~CAudioSystem()
{
    if (!alcCall(alcMakeContextCurrent, contextMadeCurrent, pDevice, nullptr))
        log_error("Failed to change current context.");

    if (!alcCall(alcDestroyContext, pDevice, openALContext))
        log_error("Failed to destroy context.");

    bool closed{ false };
    alcCall(alcCloseDevice, closed, pDevice, pDevice);
}

void CAudioSystem::create()
{
	pDevice = alcOpenDevice(nullptr);

	log_cerror(pDevice, "Could not create audio device.");

    if (!alcCall(alcCreateContext, pContext, pDevice, pDevice, nullptr) || !pContext)
        log_error("Could not create audio context");

	auto res = alcCall(alcCreateContext, pContext, pDevice, pDevice, nullptr);
	
    if (!alcCall(alcMakeContextCurrent, contextMadeCurrent, pDevice, pContext) || contextMadeCurrent != ALC_TRUE)
        log_error("Could not make audio context current.");
}

void CAudioSystem::update(float fDt)
{

}