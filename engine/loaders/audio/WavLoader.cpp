#include "WavLoader.h"

#include "system/filesystem/filesystem.h"

using namespace engine::loaders;
using namespace engine::system;

EAudioReaderError CWavReader::open(const std::string& filepath)
{
	char header[header_size];

	auto relpath = fs::get_workdir() / filepath;

	pFile = fopen(relpath.string().c_str(), "rb");
	if (!pFile)
		return EAudioReaderError::eCannotOpenFile;

	auto readed_size = fread(header, 1, header_size, pFile);
	if (readed_size != header_size)
		return EAudioReaderError::eInvalidHeader;

	rawheader = *((FWaveHeader*)header);

	fread(header, 1, 10, pFile);

	return validateHeader();;
}

size_t CWavReader::read(std::vector<char>& data)
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

void CWavReader::close()
{
	if (pFile)
	{
		fclose(pFile);
		pFile = nullptr;
	}
}

const uint16_t& CWavReader::getNumChannels() const
{
	return rawheader.numChannels;
}

const uint32_t& CWavReader::getSampleRate() const
{
	return rawheader.sampleRate;
}

const uint32_t& CWavReader::getByteRate() const
{
	return rawheader.byteRate;
}

const uint16_t& CWavReader::getBlockAlign() const
{
	return rawheader.blockAlign;
}

const uint16_t& CWavReader::getBitsPerSample() const
{
	return rawheader.bitsPerSample;
}

EAudioReaderError CWavReader::validateHeader()
{
	if (std::strncmp(rawheader.chunkID, "RIFF", 4) != 0)
		return EAudioReaderError::eInvalidHeader;

	if (std::strncmp(rawheader.format, "WAVE", 4) != 0)
		return EAudioReaderError::eInvalidHeader;

	return EAudioReaderError::eSuccess;
}