#pragma once

//#include <Effekseer.h>
//#include <EffekseerRendererVulkan.h>

namespace engine
{
	namespace particles
	{
		class CParticleRenderer
		{
		public:
			~CParticleRenderer();

			//void create(Effekseer::ManagerRef efkManager);

			void setTime(float time);
			void begin();
			void end();

			//EffekseerRenderer::RendererRef getEffekseerRenderer() { return efkRenderer; }
		private:
			//EffekseerRenderer::RendererRef efkRenderer;
			//Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> efkMemoryPool;
			//Effekseer::RefPtr<EffekseerRenderer::CommandList> efkCommandList;
		};
	}
}