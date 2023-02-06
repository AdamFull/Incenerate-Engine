#pragma once

#include "ObjectManager.hpp"
#include "ParticleObject.h"
#include "ParticleRenderer.h"

namespace engine
{
	namespace particles
	{
		class CParticlesCore
		{
		public:
			void create();
			void update(float fDT);
			void update_camera(float zNear, float zFar, const glm::mat4& view, const glm::mat4& projection);

			size_t addParticle(const std::string& name);
			size_t addParticle(const std::string& name, std::unique_ptr<CParticleObject>&& source);
			void removeParticle(const std::string& name);
			void removeParticle(size_t id);
			const std::unique_ptr<CParticleObject>& getParticle(const std::string& name);
			const std::unique_ptr<CParticleObject>& getParticle(size_t id);
		private:
			std::unique_ptr<CObjectManager<CParticleObject>> pParticleObjectManager;
			std::unique_ptr<CParticleRenderer> pRenderer;
			float fTime{ 0.f };
			float fNear{ 0.f };
			float fFar{ 1.f };

			//Effekseer::ManagerRef efkManager;
		};
	}
}