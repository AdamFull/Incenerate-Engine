#pragma once

#include <Effekseer/Effekseer.h>
#include <EffekseerRendererVulkan/EffekseerRendererVulkan.h>

namespace engine
{
	namespace particles
	{
		class CParticleRenderer
		{
		public:
			~CParticleRenderer();

			void create(Effekseer::ManagerRef efkManager);

			EffekseerRenderer::RendererRef getEffekseerRenderer() { return efkRenderer; }
		private:
			EffekseerRenderer::RendererRef efkRenderer;
			Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> efkMemoryPool;
			Effekseer::RefPtr<EffekseerRenderer::CommandList> efkCommandList;
		};
	}
}