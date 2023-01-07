#include "AudioLoader.h"

#include "audio/WavLoader.h"

using namespace engine::loaders;

std::unique_ptr<CAudioReader> CAudioLoader::load(const std::string& filepath)
{
	std::vector<char> vRaw;
	std::unique_ptr<CAudioReader> pReader;

	auto ext = filepath.substr(filepath.find_last_of(".") + 1);
	if (ext == "wav")
		pReader = std::make_unique<CWavReader>();

	pReader->open(filepath);
	return pReader;
}