#pragma once

#include "interface/AudioReaderInterface.h"

namespace engine
{
	namespace audio
	{
		class CVorbisReader : public IAudioReaderInterface
		{
		public:
			~CVorbisReader() override = default;
			EAudioReaderError open(const std::string& filepath, std::unique_ptr<FAudioSourceData>& data) override;
		};
	}
}