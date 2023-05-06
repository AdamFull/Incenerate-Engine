#pragma once

#include "interface/AudioReaderInterface.h"
#include <filesystem/interface/VirtualFileSystemInterface.h>

namespace engine
{
	namespace audio
	{
		class CWavReader : public IAudioReaderInterface
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