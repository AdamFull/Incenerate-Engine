#pragma once

#include <ie_audio_exports.h>

#include <interface/audio/AudioReaderInterface.h>
#include <interface/filesystem/VirtualFileSystemInterface.h>

namespace engine
{
	namespace audio
	{
		class INCENERATE_AUDIO_EXPORT CWavReader final : public IAudioReaderInterface
		{
		public:
			CWavReader(filesystem::IVirtualFileSystemInterface* vfs_ptr);
			~CWavReader() override = default;
			EAudioReaderError open(const std::string& filepath, std::unique_ptr<FAudioSourceData>& data) override;
		private:
			filesystem::IVirtualFileSystemInterface* m_pVFS{ nullptr };
		};
	}
}