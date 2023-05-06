#include "BaseSystem.h"

#include "Engine.h"

#include <interface/event/EventInterface.h>

using namespace engine;
using namespace engine::ecs;

void ISystem::create()
{
	registry = &EGEngine->getRegistry();
	sw.stop<float>();
	__create();
	ct = sw.stop<float>();
}

void ISystem::update(float fDt)
{
	sw.stop<float>();
	__update(fDt);
	ut = sw.stop<float>();
}

const float ISystem::createElapsed() const
{
	return ct;
}

const float ISystem::updateElapsed() const
{
	return ut;
}

const std::string& ISystem::getName() const
{
	return name;
}

const std::vector<std::unique_ptr<ISystem>>& ISystem::getSubSystems() const
{
	return vSubSystems;
}