#pragma once

#include "interface/AudioSourceInterfsce.h"
#include "interface/AudioLoaderInterface.h"

namespace engine
{
	namespace audio
	{
		class CAudioSource final : public IAudioSourceInterface
		{
		public:
			CAudioSource() = default;
			CAudioSource(IAudioLoaderInterface* loader);
			~CAudioSource() override;

			void create(const std::string& filepath) override;
			void play() override;
			void pause() override;
			void stop() override;
			void rewind() override;

			void setPitch(float value) override;
			void setGain(float value) override;
			void setPosition(const glm::vec3& position) override;
			void setVelocity(const glm::vec3& position) override;
			void setLooping(bool loop) override;

			float getLenInSec() override;
			float getPosInSec() override;
			void setOffsetSec(float offset) override;

			EAudioSourceState getState() override;
		private:
			uint32_t alBuffer{ 0 }, alSource{ 0 };
			uint32_t channels{ 0 }, sampleRate{ 0 }, bitsPerSample{ 0 };
			uint32_t byteRate{ 0 }, totalSize{ 0 };
			IAudioLoaderInterface* m_pLoader{ nullptr };
		};
	}
}