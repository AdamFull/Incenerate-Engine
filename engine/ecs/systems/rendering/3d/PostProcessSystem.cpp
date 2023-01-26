#include "PostProcessSystem.h"
#include "Engine.h"

#include "postprocess/EffectShared.h"

#include "ecs/components/CameraComponent.h"
#include "ecs/helper.hpp"

using namespace engine::graphics;
using namespace engine::ecs;
using namespace engine::system::window;

CPostProcessSystem::~CPostProcessSystem()
{
	
}

void CPostProcessSystem::__create()
{
	fxaa.create();
	dof.create();
	bloom.create();
	chromatic_aberration.create();
	vignette.create();
	tonemap.create();

	final_image = effectshared::createImage("postprocess_tex", vk::Format::eR32G32B32A32Sfloat);
	auto& image = EGGraphics->getImage(final_image);

	addSubresource("composition_tex");
	addSubresource("depth_tex");

	CBaseGraphicsSystem::__create();
}

void CPostProcessSystem::__update(float fDt)
{
	auto& device = EGGraphics->getDevice();
	auto extent = device->getExtent(true);
	auto resolution = glm::vec2(static_cast<float>(extent.width), static_cast<float>(extent.height));

	// TODO: move to engine "getActiveCamera"
	auto& registry = EGCoordinator;
	auto editorMode = EGEngine->isEditorMode();
	auto state = EGEngine->getState();

	FCameraComponent* camera{ nullptr };

	if (editorMode && state == EEngineState::eEditing)
		camera = registry.try_get<FCameraComponent>(EGEditor->getCamera());
	else
		camera = registry.try_get<FCameraComponent>(get_active_camera(registry));

	if (!camera)
		return;

	{
		effectshared::tryReCreateImage("postprocess_tex", final_image, vk::Format::eR32G32B32A32Sfloat);
		dof.update();
		bloom.update();
	}

	VkHelper::BarrierFromGraphicsToCompute(getSubresource("composition_tex"));
	VkHelper::BarrierFromGraphicsToCompute(getSubresource("depth_tex"));

	size_t current_image = getSubresource("composition_tex");
	current_image = fxaa.render(camera, current_image, final_image);
	current_image = bloom.render(camera, current_image);
	current_image = dof.render(camera, getSubresource("depth_tex"), current_image, final_image);
	current_image = chromatic_aberration.render(camera, current_image, final_image);
	current_image = vignette.render(camera, current_image, final_image);
	current_image = tonemap.render(camera, current_image, final_image);

	VkHelper::BarrierFromComputeToGraphics();
}