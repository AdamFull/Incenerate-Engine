#pragma once

#include "oalhelp.h"

namespace engine
{
	namespace audio
	{
		enum class EAudioSourceState
		{
			eInitial = AL_INITIAL,
			ePlaying = AL_PLAYING,
			ePaused = AL_PAUSED,
			eStopped = AL_STOPPED
		};

		class CAudioSource
		{
		public:
			CAudioSource() = default;
			CAudioSource(const std::string& filepath);
			~CAudioSource();

			void create(const std::string& filepath);
			void play();
			void pause();
			void stop();
			void rewind();

			void setPitch(float value);
			void setGain(float value);
			void setPosition(const glm::vec3& position);
			void setVelocity(const glm::vec3& position);
			void setLooping(bool loop);

			double getLenInSec();
			double getPosInSec();
			void setOffsetSec(float offset);

			EAudioSourceState getState();
		private:
			uint32_t alBuffer{ 0 }, alSource{ 0 };
			uint32_t channels{ 0 }, sampleRate{ 0 }, bitsPerSample{ 0 };
			uint32_t byteRate{ 0 }, totalSize{ 0 };
		};
	}
}