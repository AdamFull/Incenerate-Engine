#include "PresentRenderSystem.h"

#include "Engine.h"

using namespace engine::ecs;
using namespace engine::graphics;

void CPresentRenderSystem::__create()
{
	FShaderCreateInfo specials;
	specials.pipeline_stage = "present";
	specials.cull_mode = vk::CullModeFlagBits::eBack;
	specials.usages = 1;

	shader_id = graphics->addShader("postprocess:through_pass", specials);
}

void CPresentRenderSystem::__update(float fDt)
{
	auto stage = graphics->getRenderStageID("present");
	auto present_id = graphics->getImageID("postprocess_tex");

	graphics->bindRenderer(stage);

	graphics->bindShader(shader_id);

	graphics->bindTexture("samplerColor", present_id);;
	graphics->draw(0, 3, 0, 0, 1);

	graphics->bindShader(invalid_index);

	graphics->bindRenderer(invalid_index);
}