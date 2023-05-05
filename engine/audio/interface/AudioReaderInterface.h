#pragma once

namespace engine
{
	namespace audio
	{
		enum class EAudioReaderError
		{
			eSuccess,
			eCannotOpenFile,
			eInvalidHeader,
			eUnsupportedFMTChunkSize,
			eDataSubchunkNotFount,
			eUnknownChunkSize
		};

		struct FAudioSourceData
		{
			uint16_t channels_;
			uint32_t sample_rate_;
			uint32_t byte_rate_;
			uint16_t block_align_;
			uint16_t bits_per_sample_;
			size_t data_size_;

			//uint32_t num_samples_;  // fact
			//std::vector<uint32_t> cue_points_;  // cue
			//std::map<uint32_t, std::string> labels_;  // labl
			//std::map<uint32_t, std::string> notes_;  // note
			//uint32_t silent_duration_;  // slnt
			//std::vector<uint32_t> playlist_segments_;  // plst
			//uint8_t base_note_;  // inst

			std::unique_ptr<uint8_t[]> data_;
		};

		class IAudioReaderInterface
		{
		public:
			virtual ~IAudioReaderInterface() = default;
			virtual EAudioReaderError open(const std::string& filepath, std::unique_ptr<FAudioSourceData>& data) = 0;
		};
	}
}