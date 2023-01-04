#include "EntityManager.hpp"

#include <cassert>
#include <ranges>

using namespace engine::ecs;

CEntityManager::CEntityManager()
{
	for(Entity entity : std::views::iota(0u, MAX_ENTITIES))
		mAvailableEntities.push(entity);
}

Entity CEntityManager::create()
{
	assert(mLivingEntityCount < MAX_ENTITIES && "Too many entities in existence.");

	Entity id = mAvailableEntities.front();
	mAvailableEntities.pop();
	++mLivingEntityCount;

	return id;
}

void CEntityManager::destroy(Entity entity)
{
	assert(entity < MAX_ENTITIES && "Entity out of range.");

	mSignatures[entity].reset();
	mAvailableEntities.push(entity);
	--mLivingEntityCount;
}

void CEntityManager::setSignature(Entity entity, Signature signature)
{
	assert(entity < MAX_ENTITIES && "Entity out of range.");
	mSignatures[entity] = signature;
}

Signature CEntityManager::getSignature(Entity entity)
{
	assert(entity < MAX_ENTITIES && "Entity out of range.");
	return mSignatures[entity];
}