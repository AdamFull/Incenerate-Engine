#include "SystemManager.hpp"

using namespace engine::ecs;

void CSystemManager::entityDestroyed(Entity entity)
{
	for (auto const& [type, system] : mSystems)
		system->mEntities.erase(entity);
}

void CSystemManager::entitySignatureChanged(Entity entity, Signature entitySignature)
{
	for (auto const& [type, system] : mSystems)
	{
		auto const& systemSignature = mSignatures[type];

		if ((entitySignature & systemSignature) == systemSignature)
			system->mEntities.insert(entity);
		else
			system->mEntities.erase(entity);
	}
}