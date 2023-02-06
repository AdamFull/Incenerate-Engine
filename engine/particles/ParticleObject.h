#pragma once

//#include <Effekseer.h>

namespace engine
{
	namespace particles
	{
		class CParticleObject
		{
		public:
			//CParticleObject(Effekseer::ManagerRef manager);

			void load(const std::filesystem::path& path);

			void play(const glm::vec3& position);
			void stop();
		private:
			//Effekseer::EffectRef efkEffect;
			//Effekseer::Handle efkHandle;
			//Effekseer::ManagerRef efkManager;
		};
	}
}