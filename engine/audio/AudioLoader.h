#pragma once

#include "interface/AudioReaderInterface.h"
#include "filesystem/interface/VirtualFileSystemInterface.h"

namespace engine
{
	namespace audio
	{
		class CAudioLoader final : public IAudioLoaderInterface
		{
		public:
			EAudioReaderError load(const std::string& filepath, std::unique_ptr<FAudioSourceData>& data) override;
		private:
			filesystem::IVirtualFileSystemInterface* m_pVFS{ nullptr };
		};
	}
}