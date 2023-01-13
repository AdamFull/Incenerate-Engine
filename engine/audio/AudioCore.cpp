#include "AudioCore.h"

using namespace engine::audio;

CAudioCore::~CAudioCore()
{
    
}

void CAudioCore::create()
{
	pAudioSourceManager = std::make_unique<CObjectManager<CAudioSource>>();

    pDevice = alcOpenDevice(nullptr);

    log_cerror(pDevice, "Could not create audio device.");

    if (!alcCall(alcCreateContext, pContext, pDevice, pDevice, nullptr) || !pContext)
        log_error("Could not create audio context");

    auto res = alcCall(alcCreateContext, pContext, pDevice, pDevice, nullptr);

    if (!alcCall(alcMakeContextCurrent, contextMadeCurrent, pDevice, pContext) || contextMadeCurrent != ALC_TRUE)
        log_error("Could not make audio context current.");

    if (!alCall(alDistanceModel, AL_INVERSE_DISTANCE_CLAMPED))
        log_error("Could not set Distance Model to AL_INVERSE_DISTANCE_CLAMPED");
}

void CAudioCore::update()
{
    pAudioSourceManager->perform_deletion();
}

void CAudioCore::shutdown()
{
    pAudioSourceManager = nullptr;

    if (!alcCall(alcMakeContextCurrent, contextMadeCurrent, pDevice, nullptr))
        log_error("Failed to change current context.");

    if (!alcCall(alcDestroyContext, pDevice, pContext))
        log_error("Failed to destroy context.");

    bool closed{ false };
    alcCall(alcCloseDevice, closed, pDevice, pDevice);
}

size_t CAudioCore::addSource(const std::string& name, const std::string& filepath)
{
    return pAudioSourceManager->add(name, std::make_unique<CAudioSource>(filepath));
}

size_t CAudioCore::addSource(const std::string& name, std::unique_ptr<CAudioSource>&& source)
{
    return pAudioSourceManager->add(name, std::move(source));
}

void CAudioCore::removeSource(const std::string& name)
{
    pAudioSourceManager->remove(name);
}

void CAudioCore::removeSource(size_t id)
{
    pAudioSourceManager->remove(id);
}

const std::unique_ptr<CAudioSource>& CAudioCore::getSource(const std::string& name)
{
    return pAudioSourceManager->get(name);
}

const std::unique_ptr<CAudioSource>& CAudioCore::getSource(size_t id)
{
    return pAudioSourceManager->get(id);
}