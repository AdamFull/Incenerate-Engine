#pragma once

#include <ie_audio_exports.h>

#include "interface/AudioCoreInterface.h"
#include <shared/ObjectManager.hpp>
#include <interface/filesystem/VirtualFileSystemInterface.h>

struct ALCdevice_struct;
struct ALCcontext_struct;

namespace engine
{
	namespace audio
	{
		class INCENERATE_AUDIO_EXPORT CAudioCore final : public IAudioSystemInterface
		{
		public:
			CAudioCore(filesystem::IVirtualFileSystemInterface* vfs_ptr);
			~CAudioCore() override;

			void create() override;
			void update(float fDt) override;
			void shutdown() override;

			void setListenerPosition(const glm::vec3& position, const glm::vec3& forward_vector, const glm::vec3& right_vector) override;

			size_t addSource(const std::string& name, const std::string& filepath) override;
			size_t addSource(const std::string& name, std::unique_ptr<IAudioSourceInterface>&& source) override;
			void removeSource(const std::string& name) override;
			void removeSource(size_t id) override;
			const std::unique_ptr<IAudioSourceInterface>& getSource(const std::string& name) override;
			const std::unique_ptr<IAudioSourceInterface>& getSource(size_t id) override;
		private:
			ALCdevice_struct* pDevice{ nullptr };
			ALCcontext_struct* pContext{ nullptr };
			int8_t contextMadeCurrent{ false };
			std::unique_ptr<CObjectManager<IAudioSourceInterface>> pAudioSourceManager;
			filesystem::IVirtualFileSystemInterface* m_pVFS{ nullptr };
		};
	}
}