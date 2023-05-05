#include "WavLoader.h"

#include "Engine.h"

#include "filesystem/vfs_helper.h"

using namespace engine::audio;
using namespace engine::filesystem;

EAudioReaderError CWavReader::open(const std::string& filepath, std::unique_ptr<FAudioSourceData>& data)
{
	data = std::make_unique<FAudioSourceData>();

	auto pFile = EGFilesystem->openFile(filepath);
	if (!pFile->is_open())
		return EAudioReaderError::eCannotOpenFile;

	char header[48ull];
	pFile->read(header, 12ull);

	if (std::strncmp(header, "RIFF", 4ull) != 0 || std::strncmp(header + 8ull, "WAVE", 4ull) != 0)
		return EAudioReaderError::eInvalidHeader;

	while (!pFile->eof())
	{
		pFile->read(header, 8ull);

		uint32_t chunk_size = *reinterpret_cast<uint32_t*>(header + 4);

		if (std::strncmp(header, "fmt ", 4) == 0) 
		{
			if (chunk_size < 16 || chunk_size > 18) 
				return EAudioReaderError::eUnsupportedFMTChunkSize;

			pFile->read(header, chunk_size);
			data->channels_ = *reinterpret_cast<uint16_t*>(header + 2);
			data->sample_rate_ = *reinterpret_cast<uint32_t*>(header + 4);
			data->byte_rate_ = *reinterpret_cast<uint32_t*>(header + 8);
			data->block_align_ = *reinterpret_cast<uint16_t*>(header + 12);
			data->bits_per_sample_ = *reinterpret_cast<uint16_t*>(header + 14);
		}
		else if (std::strncmp(header, "data", 4) == 0) 
		{
			data->data_size_ = chunk_size;
			data->data_ = std::make_unique<uint8_t[]>(data->data_size_);
			pFile->read(reinterpret_cast<char*>(data->data_.get()), data->data_size_);
			break;
		}
		else if (std::strncmp(header, "fact", 4) == 0) 
		{
			//pFile->read(reinterpret_cast<char*>(&data->num_samples_), 4);
			//pFile->seekg(chunk_size - 4, std::ios_base::cur);
			pFile->seekg(chunk_size, std::ios_base::cur);
		}
		else if (std::strncmp(header, "cue ", 4) == 0) 
		{
			//uint32_t num_cue_points;
			//pFile->read(reinterpret_cast<char*>(&num_cue_points), 4);
			//data->cue_points_.resize(num_cue_points);
			//for (uint32_t i = 0; i < num_cue_points; ++i) 
			//{
			//	uint32_t cue_point;
			//	pFile->seekg(4, std::ios_base::cur);
			//	pFile->read(reinterpret_cast<char*>(&cue_point), 4);
			//	data->cue_points_[i] = cue_point;
			//	pFile->seekg(12, std::ios_base::cur);
			//}
			//pFile->seekg(chunk_size - 4 - num_cue_points * 24, std::ios_base::cur);
			pFile->seekg(chunk_size, std::ios_base::cur);
		}
		else if (std::strncmp(header, "labl", 4) == 0) 
		{
			//uint32_t label_id;
			//pFile->read(reinterpret_cast<char*>(&label_id), 4);
			//std::vector<char> label_data(chunk_size - 4);
			//pFile->read(label_data.data(), chunk_size - 4);
			//std::string label(label_data.data(), label_data.size());
			//data->labels_[label_id] = label;
			pFile->seekg(chunk_size, std::ios_base::cur);
		}
		else if (std::strncmp(header, "smpl", 4) == 0) 
		{
			pFile->seekg(chunk_size, std::ios_base::cur);
		}
		else if (std::strncmp(header, "slnt", 4) == 0) 
		{
			//pFile->read(reinterpret_cast<char*>(&data->silent_duration_), 4);
			//pFile->seekg(chunk_size - 4, std::ios_base::cur);
			pFile->seekg(chunk_size, std::ios_base::cur);
		}
		else if (std::strncmp(header, "plst", 4) == 0) 
		{
			//uint32_t num_segments;
			//pFile->read(reinterpret_cast<char*>(&num_segments), 4);
			//data->playlist_segments_.resize(num_segments);
			//for (uint32_t i = 0; i < num_segments; ++i) 
			//{
			//	uint32_t segment_start;
			//	pFile->read(reinterpret_cast<char*>(&segment_start), 4);
			//	data->playlist_segments_[i] = segment_start;
			//	pFile->seekg(4, std::ios_base::cur);
			//}
			//pFile->seekg(chunk_size - 4 - num_segments * 8, std::ios_base::cur);
			pFile->seekg(chunk_size, std::ios_base::cur);
		}
		else if (std::strncmp(header, "inst", 4) == 0) 
		{
			//pFile->read(reinterpret_cast<char*>(&data->base_note_), 1);
			//pFile->seekg(chunk_size - 1, std::ios_base::cur);
			pFile->seekg(chunk_size, std::ios_base::cur);
		}
		else if (std::strncmp(header, "LIST", 4) == 0) 
		{
			pFile->seekg(chunk_size, std::ios_base::cur);
		}
		else 
			return EAudioReaderError::eUnknownChunkSize;
	}

	return EAudioReaderError::eSuccess;
}