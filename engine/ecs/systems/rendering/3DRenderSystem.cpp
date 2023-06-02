#include "3DRenderSystem.h"

#include "Engine.h"

#include "3d/shadows/ShadowPassSystem.h"
#include "3d/DeferredPassSystem.h"
#include "3d/SSAOSystem.h"
#include "3d/ReflectionsSystem.h"
#include "3d/CompositionSystem.h"
#include "3d/PostProcessSystem.h"

using namespace engine::graphics;
using namespace engine::ecs;

void C3DRenderSystem::__create()
{
	vSubSystems.emplace_back(std::make_unique<CShadowPassSystem>());
	vSubSystems.emplace_back(std::make_unique<CDeferredPassSystem>());
	vSubSystems.emplace_back(std::make_unique<CSSAOSystem>());
	vSubSystems.emplace_back(std::make_unique<CReflectionsSystem>());
	vSubSystems.emplace_back(std::make_unique<CCompositionSystem>());
	vSubSystems.emplace_back(std::make_unique<CPostProcessSystem>());

	for (auto& system : vSubSystems)
		system->create();
}

void C3DRenderSystem::__update(float fDt)
{
	for (auto& system : vSubSystems)
		system->update(fDt);
}