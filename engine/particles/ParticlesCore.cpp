#include "ParticlesCore.h"

#include <Effekseer/Effekseer.h>

using namespace engine::particles;

void CParticlesCore::create()
{
	pParticleObjectManager = std::make_unique<CObjectManager<CParticleObject>>();

	pRenderer = std::make_unique<CParticleRenderer>();

	efkManager = Effekseer::Manager::Create(8000);

	pRenderer->create(efkManager);
}

void CParticlesCore::update(float fDT)
{
	pParticleObjectManager->perform_deletion();
}

size_t CParticlesCore::addParticle(const std::string& name)
{
	return pParticleObjectManager->add(name, std::make_unique<CParticleObject>());
}

size_t CParticlesCore::addParticle(const std::string& name, std::unique_ptr<CParticleObject>&& source)
{
	return pParticleObjectManager->add(name, std::move(source));
}

void CParticlesCore::removeParticle(const std::string& name)
{
	pParticleObjectManager->remove(name);
}

void CParticlesCore::removeParticle(size_t id)
{
	pParticleObjectManager->remove(id);
}

const std::unique_ptr<CParticleObject>& CParticlesCore::getParticle(const std::string& name)
{
	return pParticleObjectManager->get(name);
}

const std::unique_ptr<CParticleObject>& CParticlesCore::getParticle(size_t id)
{
	return pParticleObjectManager->get(id);
}