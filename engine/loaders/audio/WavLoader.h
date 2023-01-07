#pragma once

#include "AudioReader.h"

namespace engine
{
	namespace loaders
	{
		struct  FWaveHeader
		{
			char            chunkID[4];                // RIFF Header      Magic header
			uint32_t        chunkSize{ 0 };         // RIFF Chunk Size  
			char            format[4];                // WAVE Header      
			char            subchunk1ID[4];                 // FMT header       
			uint32_t        subchunk1Size{ 0 };     // Size of the fmt chunk                                
			uint16_t        audioFormat{ 0 };       // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM 
			uint16_t        numChannels{ 0 };     // Number of channels 1=Mono 2=Sterio                   
			uint32_t        sampleRate{ 0 };     // Sampling Frequency in Hz                             
			uint32_t        byteRate{ 0 };       // bytes per second 
			uint16_t        blockAlign{ 0 };        // 2=16-bit mono, 4=16-bit stereo 
			uint16_t        bitsPerSample{ 0 };     // Number of bits per sample      
			char            subchunk2ID[4];         // "data"  string   
			uint32_t        subchunk2Size{ 0 };     // Sampled data length    
		};

		class CWavReader : public CAudioReader
		{
		public:
			virtual ~CWavReader() override = default;
			EAudioReaderError open(const std::string& filepath) override;
			size_t read(std::vector<char>& data) override;
			void close() override;

			const uint16_t& getNumChannels() const override;
			const uint32_t& getSampleRate() const override;
			const uint32_t& getByteRate() const override;
			const uint16_t& getBlockAlign() const override;
			const uint16_t& getBitsPerSample() const override;
		private:
			EAudioReaderError validateHeader();
			static constexpr size_t header_size{ sizeof(FWaveHeader) };
			FWaveHeader rawheader;
			FILE* pFile{ nullptr };
		};
	}
}