#include "DeferredPassSystem.h"

#include "Engine.h"

#include "EnvironmentSystem.h"
#include "MeshSystem.h"

using namespace engine::graphics;
using namespace engine::ecs;

void CDeferredPassSystem::__create()
{
	vSubSystems.emplace_back(std::make_unique<CEnvironmentSystem>());
	vSubSystems.emplace_back(std::make_unique<CMeshSystem>());

	for (auto& system : vSubSystems)
		system->create();
}

void CDeferredPassSystem::__update(float fDt)
{
	auto commandBuffer = EGGraphics->getCommandBuffer();
	auto& stage = EGGraphics->getRenderStage("deferred");

	stage->begin(commandBuffer);

	for (auto& system : vSubSystems)
		system->update(fDt);

	stage->end(commandBuffer);
}