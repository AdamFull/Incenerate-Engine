#include "VorbisLoader.h"

#include "Engine.h"

#include "stb_vorbis.h"

using namespace engine::audio;

EAudioReaderError CVorbisReader::open(const std::string& filepath, std::unique_ptr<FAudioSourceData>& data)
{
	data = std::make_unique<FAudioSourceData>();

	std::vector<uint8_t> filedata;
	if (!EGFilesystem->readFile(filepath, filedata))
		return EAudioReaderError::eCannotOpenFile;

	int error{};
	auto vorbis = stb_vorbis_open_memory(filedata.data(), filedata.size(), &error, NULL);

	if (!vorbis)
		return EAudioReaderError::eInvalidHeader;

	stb_vorbis_info info = stb_vorbis_get_info(vorbis);
	data->channels_ = info.channels;
	data->sample_rate_ = info.sample_rate;
	data->byte_rate_ = info.sample_rate * info.channels * 2; // 2 bytes per sample for 16-bit audio
	data->block_align_ = info.channels * 2; // 2 bytes per sample for 16-bit audio
	data->bits_per_sample_ = 16; // Assuming 16-bit audio

	int num_samples = stb_vorbis_stream_length_in_samples(vorbis);
	data->data_size_ = num_samples * data->channels_ * sizeof(int16_t);
	data->data_ = std::make_unique<uint8_t[]>(data->data_size_);

	int samples_decoded = stb_vorbis_get_samples_short_interleaved(vorbis, data->channels_, (int16_t*)data->data_.get(), num_samples);

	if (samples_decoded != num_samples) 
	{
		stb_vorbis_close(vorbis);
		return EAudioReaderError::eUnknownChunkSize;
	}

	stb_vorbis_close(vorbis);


	return EAudioReaderError::eSuccess;
}