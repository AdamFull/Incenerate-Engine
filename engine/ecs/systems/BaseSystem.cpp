#include "BaseSystem.h"

using namespace engine::ecs;

void ISystem::create()
{
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