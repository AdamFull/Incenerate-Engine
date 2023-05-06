#pragma once

#include <ie_audio_exports.h>

#include <interface/audio/AudioReaderInterface.h>
#include <interface/filesystem/VirtualFileSystemInterface.h>

namespace engine
{
	namespace audio
	{
		class INCENERATE_AUDIO_EXPORT CVorbisReader final : public IAudioReaderInterface
		{
		public:
			CVorbisReader(filesystem::IVirtualFileSystemInterface* vfs_ptr);
			~CVorbisReader() override = default;
			EAudioReaderError open(const std::string& filepath, std::unique_ptr<FAudioSourceData>& data) override;
		private:
			filesystem::IVirtualFileSystemInterface* m_pVFS{ nullptr };
		};
	}
}