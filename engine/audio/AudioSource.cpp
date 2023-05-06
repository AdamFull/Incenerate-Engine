#include "AudioSource.h"

#include "loaders/WavLoader.h"
#include "loaders/VorbisLoader.h"

#include "oalhelp.h"

#include <filesystem/vfs_helper.h>

#include <logger/logger.h>

using namespace engine::audio;
using namespace engine::filesystem;

CAudioSource::CAudioSource(IVirtualFileSystemInterface* vfs_ptr) :
	m_pVFS(vfs_ptr)
{
}

CAudioSource::~CAudioSource()
{
	stop();
	alCall(alDeleteSources, 1, &alSource);
	alCall(alDeleteBuffers, 1, &alBuffer);
}

void CAudioSource::create(const std::string& filepath)
{
	alCall(alGenBuffers, 1, &alBuffer);

	std::unique_ptr<FAudioSourceData> pData;
	std::unique_ptr<IAudioReaderInterface> pReader;

	auto ext = fs::get_ext(filepath);
	if (ext == ".wav")
		pReader = std::make_unique<CWavReader>(m_pVFS);
	else if (ext == ".ogg")
		pReader = std::make_unique<CVorbisReader>(m_pVFS);

	auto result = pReader->open(filepath, pData);

	channels = pData->channels_;
	sampleRate = pData->sample_rate_;
	bitsPerSample = pData->bits_per_sample_;
	byteRate = pData->byte_rate_;
	totalSize = pData->data_size_;

	ALenum format;
	if (channels == 1 && bitsPerSample == 8)
		format = AL_FORMAT_MONO8;
	else if (channels == 1 && bitsPerSample == 16)
		format = AL_FORMAT_MONO16;
	else if (channels == 2 && bitsPerSample == 8)
		format = AL_FORMAT_STEREO8;
	else if (channels == 2 && bitsPerSample == 16)
		format = AL_FORMAT_STEREO16;
	else
	{
		log_error("Unknown audio format: {} channels, {} bps.", channels, bitsPerSample);
		return;
	}

	alCall(alBufferData, alBuffer, format, reinterpret_cast<char*>(pData->data_.get()), totalSize, sampleRate);

	alCall(alGenSources, 1, &alSource);
	alCall(alSourcei, alSource, AL_BUFFER, alBuffer);

	setPitch(1.f);
	setGain(1.f);
	setPosition({ 0.f, 0.f, 0.f });
	setVelocity({0.f, 0.f, 0.f});
	setLooping(false);
}

void CAudioSource::play()
{
	alCall(alSourcePlay, alSource);
}

void CAudioSource::pause()
{
	alCall(alSourcePause, alSource);
}

void CAudioSource::stop()
{
	alCall(alSourceStop, alSource);
}

void CAudioSource::rewind()
{
	alCall(alSourceRewind, alSource);
}

void CAudioSource::setPitch(float value)
{
	alCall(alSourcef, alSource, AL_PITCH, value);
}

void CAudioSource::setGain(float value)
{
	alCall(alSourcef, alSource, AL_GAIN, value);
}

void CAudioSource::setPosition(const glm::vec3& position)
{
	alCall(alSource3f, alSource, AL_POSITION, position.x, position.y, position.z);
}

void CAudioSource::setVelocity(const glm::vec3& position)
{
	alCall(alSource3f, alSource, AL_VELOCITY, position.x, position.y, position.z);
}

void CAudioSource::setLooping(bool loop)
{
	alCall(alSourcei, alSource, AL_LOOPING, loop);
}

float CAudioSource::getLenInSec()
{
	auto rt = static_cast<float>(byteRate);
	return static_cast<float>(totalSize) / rt;
}

float CAudioSource::getPosInSec()
{
	float offset;
	alCall(alGetSourcef, alSource, AL_SEC_OFFSET, &offset);
	return offset;
}

void CAudioSource::setOffsetSec(float offset)
{
	alCall(alSourcef, alSource, AL_SEC_OFFSET, offset);
}

EAudioSourceState CAudioSource::getState()
{
	int32_t state{ 0 };
	alCall(alGetSourcei, alSource, AL_SOURCE_STATE, &state);
	return static_cast<EAudioSourceState>(state);
}