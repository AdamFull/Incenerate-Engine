#pragma once

#include "AudioSource.h"
#include "ObjectManager.hpp"

namespace engine
{
	namespace audio
	{
		class CAudioCore
		{
		public:
			~CAudioCore();

			void create();
			void update();
			void shutdown();

			size_t addSource(const std::string& name, const std::filesystem::path& filepath);
			size_t addSource(const std::string& name, std::unique_ptr<CAudioSource>&& source);
			void removeSource(const std::string& name);
			void removeSource(size_t id);
			const std::unique_ptr<CAudioSource>& getSource(const std::string& name);
			const std::unique_ptr<CAudioSource>& getSource(size_t id);
		private:
			ALCdevice* pDevice{ nullptr };
			ALCcontext* pContext{ nullptr };
			int8_t contextMadeCurrent{ false };
			std::unique_ptr<CObjectManager<CAudioSource>> pAudioSourceManager;
		};
	}
}