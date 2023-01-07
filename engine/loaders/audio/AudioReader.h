#pragma once

#include <vector>

namespace engine
{
	namespace loaders
	{
		enum class EAudioReaderError
		{
			eCannotOpenFile,
			eInvalidHeader,
			eSuccess
		};

		class CAudioReader
		{
		public:
			virtual ~CAudioReader()
			{
				close();
			}

			virtual EAudioReaderError open(const std::string& filepath) = 0;
			virtual size_t read(std::vector<char>& data) = 0;
			virtual void close() {};

			virtual const uint16_t& getNumChannels() const = 0;
			virtual const uint32_t& getSampleRate() const = 0;
			virtual const uint32_t& getByteRate() const = 0;
			virtual const uint16_t& getBlockAlign() const = 0;
			virtual const uint16_t& getBitsPerSample() const = 0;
		};
	}
}