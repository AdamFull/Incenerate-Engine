#pragma once

#include "AudioReaderInterface.h"

namespace engine
{
	namespace audio
	{
		class IAudioLoaderInterface
		{
		public:
			virtual ~IAudioLoaderInterface() = default;

			virtual EAudioReaderError load(const std::string& filepath, std::unique_ptr<FAudioSourceData>& data) = 0;
		};
	}
}