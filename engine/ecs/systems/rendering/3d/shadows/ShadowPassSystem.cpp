#include "ShadowPassSystem.h"

#include "Engine.h"

using namespace engine::ecs;

void CShadowPassSystem::__create()
{
	vSubSystems.emplace_back(std::make_unique<CDirectionalShadowSystem>());
	vSubSystems.emplace_back(std::make_unique<COmniShadowSystem>());

	for (auto& system : vSubSystems)
		system->create();

	CBaseGraphicsSystem::__create();
}

void CShadowPassSystem::__update(float fDt)
{
	auto& stage = EGGraphics->getRenderStage("shadow");
	auto commandBuffer = EGGraphics->getCommandBuffer();

	stage->begin(commandBuffer);

	uint32_t subpasses{ 0 };
	for (auto& system : vSubSystems)
	{
		system->update(fDt);
		if(subpasses < vSubSystems.size() - 1)
			commandBuffer.nextSubpass(vk::SubpassContents::eInline);
		subpasses++;
	}

	stage->end(commandBuffer);
}