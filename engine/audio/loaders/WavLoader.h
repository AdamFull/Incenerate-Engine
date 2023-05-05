#pragma once

#include "interface/AudioReaderInterface.h"

namespace engine
{
	namespace audio
	{
		class CWavReader : public IAudioReaderInterface
		{
		public:
			~CWavReader() override = default;
			EAudioReaderError open(const std::string& filepath, std::unique_ptr<FAudioSourceData>& data) override;
		};
	}
}