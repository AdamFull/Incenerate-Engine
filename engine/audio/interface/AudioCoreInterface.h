#pragma once

#include "AudioSourceInterfsce.h"

namespace engine
{
	namespace audio
	{
		class IAudioSystemInterface
		{
		public:
			virtual ~IAudioSystemInterface() = default;

			virtual void create() = 0;
			virtual void update(float fDt) = 0;
			virtual void shutdown() = 0;

			virtual void setListenerPosition(const glm::vec3& position, const glm::vec3& forward_vector, const glm::vec3& right_vector) = 0;

			virtual size_t addSource(const std::string& name, const std::string& filepath) = 0;
			virtual size_t addSource(const std::string& name, std::unique_ptr<IAudioSourceInterface>&& source) = 0;
			virtual void removeSource(const std::string& name) = 0;
			virtual void removeSource(size_t id) = 0;
			virtual const std::unique_ptr<IAudioSourceInterface>& getSource(const std::string& name) = 0;
			virtual const std::unique_ptr<IAudioSourceInterface>& getSource(size_t id) = 0;
		};
	}
}