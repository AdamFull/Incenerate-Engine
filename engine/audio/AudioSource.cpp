#include "AudioSource.h"

#include "loaders/AudioLoader.h"
#include "oalhelp.h"

using namespace engine::audio;
using namespace engine::loaders;

CAudioSource::CAudioSource(const std::string& filepath)
{
	create(filepath);
}

CAudioSource::~CAudioSource()
{
	alCall(alDeleteSources, 1, &alSource);
	alCall(alDeleteBuffers, 1, &alBuffer);
}

void CAudioSource::create(const std::string& filepath)
{
	std::vector<char> vRawBuffer;

	alCall(alGenBuffers, 1, &alBuffer);

	auto loader = CAudioLoader::load(filepath);
	loader->read(vRawBuffer);

	channels = loader->getNumChannels();
	sampleRate = loader->getSampleRate();
	bitsPerSample = loader->getBitsPerSample();
	byteRate = loader->getByteRate();
	totalSize = vRawBuffer.size();

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

	alCall(alBufferData, alBuffer, format, vRawBuffer.data(), vRawBuffer.size(), sampleRate);

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
	alCall(alSourceStop, alSource);
}

void CAudioSource::stop()
{
	alCall(alSourceRewind, alSource);
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

double CAudioSource::getLenInSec()
{
	double byteRate = static_cast<double>(byteRate);
	return static_cast<double>(totalSize) / byteRate;
}

double CAudioSource::getPosInSec()
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