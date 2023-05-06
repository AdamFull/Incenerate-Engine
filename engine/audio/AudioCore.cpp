#include "AudioCore.h"

#include "AudioSource.h"

#include "logger/logger.h"

#include "oalhelp.h"

using namespace engine::audio;
using namespace engine::filesystem;

CAudioCore::CAudioCore(IVirtualFileSystemInterface* vfs_ptr) :
    m_pVFS(vfs_ptr)
{

}

CAudioCore::~CAudioCore()
{
    
}

void CAudioCore::create()
{
	pAudioSourceManager = std::make_unique<CObjectManager<IAudioSourceInterface>>();

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

void CAudioCore::update(float fDt)
{
    pAudioSourceManager->performDeletion();
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

void CAudioCore::setListenerPosition(const glm::vec3& position, const glm::vec3& forward_vector, const glm::vec3& right_vector)
{
    alCall(alListener3f, AL_POSITION, position.x, position.y, position.z);
    float orient[6] = { forward_vector.x, forward_vector.y, forward_vector.z, right_vector.x, right_vector.y, right_vector.z };
    alCall(alListenerfv, AL_ORIENTATION, orient);
}

size_t CAudioCore::addSource(const std::string& name, const std::string& filepath)
{
    auto file = std::make_unique<CAudioSource>(m_pVFS);
    file->create(filepath);
    return pAudioSourceManager->add(name, std::move(file));
}

size_t CAudioCore::addSource(const std::string& name, std::unique_ptr<IAudioSourceInterface>&& source)
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

const std::unique_ptr<IAudioSourceInterface>& CAudioCore::getSource(const std::string& name)
{
    return pAudioSourceManager->get(name);
}

const std::unique_ptr<IAudioSourceInterface>& CAudioCore::getSource(size_t id)
{
    return pAudioSourceManager->get(id);
}