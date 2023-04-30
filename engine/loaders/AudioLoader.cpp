#include "AudioLoader.h"

#include "audio/WavLoader.h"
#include "filesystem/vfs_helper.h"

using namespace engine::filesystem;
using namespace engine::loaders;

std::unique_ptr<CAudioReader> CAudioLoader::load(const std::string& filepath)
{
	std::vector<char> vRaw;
	std::unique_ptr<CAudioReader> pReader;

	auto ext = fs::get_ext(filepath);
	if (ext == ".wav")
		pReader = std::make_unique<CWavReader>();

	pReader->open(filepath);
	return pReader;
}