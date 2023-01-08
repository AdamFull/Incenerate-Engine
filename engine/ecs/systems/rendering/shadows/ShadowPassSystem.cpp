#include "ShadowPassSystem.h"

#include "Engine.h"

using namespace engine::ecs;

void CShadowPassSystem::__create()
{
	vSubSystems.emplace_back(std::make_unique<CDirectionalShadowSystem>());
	vSubSystems.emplace_back(std::make_unique<COmniShadowSystem>());

	for (auto& system : vSubSystems)
		system->create();
}

void CShadowPassSystem::__update(float fDt)
{
	auto& stage = EGGraphics->getRenderStage("shadow");
	auto commandBuffer = EGGraphics->getCommandBuffer();

	stage->begin(commandBuffer);

	for (auto& system : vSubSystems)
		system->update(fDt);

	stage->end(commandBuffer);
}