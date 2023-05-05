#include "AudioLoader.h"

#include "loaders/WavLoader.h"
#include "loaders/VorbisLoader.h"
#include "filesystem/vfs_helper.h"

using namespace engine::filesystem;
using namespace engine::audio;

EAudioReaderError CAudioLoader::load(const std::string& filepath, std::unique_ptr<FAudioSourceData>& data)
{
	std::vector<char> vRaw;
	std::unique_ptr<IAudioReaderInterface> pReader;

	auto ext = fs::get_ext(filepath);
	if (ext == ".wav")
		pReader = std::make_unique<CWavReader>();
	else if (ext == ".ogg")
		pReader = std::make_unique<CVorbisReader>();

	return pReader->open(filepath, data);
}