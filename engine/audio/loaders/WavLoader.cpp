#include "WavLoader.h"

using namespace engine::audio;

EAudioReaderError CWavLoader::open(const std::string& filepath)
{
	char header[header_size];

	pFile = fopen(filename.c_str(), "rb");
	if (!pFile)
		return EAudioReaderError::eCannotOpenFile;

	auto readed_size = fread(header, 1, header_size, pFile);
	if (readed_size != header_size)
		return EAudioReaderError::eInvalidHeader;

	rawheader = *((FWaveHeader*)header);

	fread(header, 1, 10, pFile);

	return validateHeader();;
}

size_t CWavLoader::read(std::vector<char>& data)
{
	bool bReaded{ false };
	std::vector<char> tmp;

	size_t readed{ 0 }, whole_size{ 0 };
	while (!feof(pFile))
	{
		tmp.resize(rawheader.chunkSize);
		readed = fread(&tmp[0], 1, tmp.size(), pFile);
		std::copy(tmp.begin(), tmp.begin() + readed, std::back_inserter(data));
		whole_size += readed;
	}

	return whole_size;
}

void CWavLoader::close()
{
	if (pFile)
	{
		fclose(pFile);
		pFile = nullptr;
	}
}

const uint16_t& CWavLoader::getNumChannels() const
{
	return rawheader.numChannels;
}

const uint32_t& CWavLoader::getSampleRate() const
{
	rawheader.sampleRate;
}

const uint32_t& CWavLoader::getByteRate() const
{
	rawheader.byteRate;
}

const uint16_t& CWavLoader::getBlockAlign() const
{
	rawheader.blockAlign;
}

const uint16_t& CWavLoader::getBitsPerSample() const
{
	return rawheader.bitsPerSample;
}

EAudioReaderError CWavLoader::validateHeader()
{
	if (std::strncmp(rawheader.chunkID, "RIFF", 4) != 0)
		return EAudioReaderError::eInvalidHeader;

	if (std::strncmp(rawheader.format, "WAVE", 4) != 0)
		return EAudioReaderError::eInvalidHeader;

	return EAudioReaderError::eSuccess;
}