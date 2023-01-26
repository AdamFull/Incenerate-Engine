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

	CBaseGraphicsSystem::__create();
}

void CDeferredPassSystem::__update(float fDt)
{
	auto& graphics = EGGraphics;
	auto stage = graphics->getRenderStageID("deferred");

	graphics->bindRenderer(stage);

	for (auto& system : vSubSystems)
		system->update(fDt);

	graphics->bindRenderer(invalid_index);
}