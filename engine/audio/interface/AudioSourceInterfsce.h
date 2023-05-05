#pragma once

namespace engine
{
	namespace audio
	{
		enum class EAudioSourceState
		{
			eInitial = 0x1011,
			ePlaying = 0x1012,
			ePaused = 0x1013,
			eStopped = 0x1014
		};

		class IAudioSourceInterface
		{
		public:
			virtual ~IAudioSourceInterface() = default;

			virtual void create(const std::string& filepath) = 0;
			virtual void play() = 0;
			virtual void pause() = 0;
			virtual void stop() = 0;
			virtual void rewind() = 0;

			virtual void setPitch(float value) = 0;
			virtual void setGain(float value) = 0;
			virtual void setPosition(const glm::vec3& position) = 0;
			virtual void setVelocity(const glm::vec3& position) = 0;
			virtual void setLooping(bool loop) = 0;

			virtual float getLenInSec() = 0;
			virtual float getPosInSec() = 0;
			virtual void setOffsetSec(float offset) = 0;

			virtual EAudioSourceState getState() = 0;
		};
	}
}