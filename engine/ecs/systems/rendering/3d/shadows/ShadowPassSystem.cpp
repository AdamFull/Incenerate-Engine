#include "ShadowPassSystem.h"

#include "Engine.h"

using namespace engine::ecs;

void CShadowPassSystem::__create()
{
	vSubSystems.emplace_back(std::make_unique<CCascadeShadowSystem>());
	vSubSystems.emplace_back(std::make_unique<CDirectionalShadowSystem>());
	vSubSystems.emplace_back(std::make_unique<COmniShadowSystem>());

	for (auto& system : vSubSystems)
		system->create();

	CBaseGraphicsSystem::__create();
}

void CShadowPassSystem::__update(float fDt)
{
	// TODO: remove commandBuffer here
	auto commandBuffer = graphics->getCommandBuffer();

	uint32_t subpasses{ 0 };
	for (auto& system : vSubSystems)
	{
		system->update(fDt);
		subpasses++;
	}
}