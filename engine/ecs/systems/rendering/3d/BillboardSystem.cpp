#include "BillboardSystem.h"

#include "Engine.h"

#include "graphics/image/Image.h"
#include "loaders/ImageLoader.h"

#include "ecs/components/components.h"
#include "ecs/helper.hpp"

#include <Helpers.h>

using namespace engine::ecs;
using namespace engine::graphics;
using namespace engine::loaders;

void CBillboardSystem::__create()
{
	FShaderSpecials specials;
	specials.usages = 4;
	shader_id = graphics->addShader("billboard", "billboard", specials);
	vbo_id = graphics->addVertexBuffer("billboard");

	auto& pVBO = graphics->getVertexBuffer(vbo_id);
	pVBO->addPrimitive(std::make_unique<FQuadPrimitive>());
	pVBO->create();

	if (EGEngine->isEditorMode())
	{
		auto& device = graphics->getDevice();

		auto image = std::make_unique<CImage>(device.get());
		image->create("icon/editor/audio.png", vk::Format::eR8G8B8A8Srgb);
		audio_icon = graphics->addImage("editor_audio_billboard", std::move(image));

		image = std::make_unique<CImage>(device.get());
		image->create("icon/editor/camera.png", vk::Format::eR8G8B8A8Srgb);
		camera_icon = graphics->addImage("editor_camera_billboard", std::move(image));

		image = std::make_unique<CImage>(device.get());
		image->create("icon/editor/environment.png", vk::Format::eR8G8B8A8Srgb);
		environment_icon = graphics->addImage("editor_environment_billboard", std::move(image));

		image = std::make_unique<CImage>(device.get());
		image->create("icon/editor/light.png", vk::Format::eR8G8B8A8Srgb);
		light_icon = graphics->addImage("editor_light_billboard", std::move(image));
	}
}

void CBillboardSystem::__update(float fDt)
{
	auto& registry = EGEngine->getRegistry();

	auto* camera = EGEngine->getActiveCamera();

	if (!camera)
		return;

	graphics->bindShader(shader_id);
	graphics->setManualShaderControlFlag(true);
	graphics->bindVertexBuffer(vbo_id);

	auto& pPush = graphics->getPushBlockHandle("billboard");
	pPush->set("projection", camera->projection);
	pPush->set("view", camera->view);

	if (EGEngine->isEditorEditing())
	{
		{
			graphics->bindTexture("color_tex", audio_icon);
			graphics->flushShader();

			auto view = registry.view<FTransformComponent, FAudioComponent>();
			for (auto [entity, transform, audio] : view.each())
				draw(camera, transform.rposition, glm::vec3(1.f), entity);
		}

		{
			graphics->bindTexture("color_tex", camera_icon);
			graphics->flushShader();

			auto view = registry.view<FTransformComponent, FCameraComponent>();
			for (auto [entity, transform, cam] : view.each())
				draw(camera, transform.rposition, glm::vec3(1.f), entity);
		}

		{
			graphics->bindTexture("color_tex", environment_icon);
			graphics->flushShader();

			auto view = registry.view<FTransformComponent, FEnvironmentComponent>();
			for (auto [entity, transform, env] : view.each())
				draw(camera, transform.rposition, glm::vec3(1.f), entity);
		}

		{
			graphics->bindTexture("color_tex", light_icon);
			graphics->flushShader();

			auto dir = registry.view<FTransformComponent, FDirectionalLightComponent>();
			for (auto [entity, transform, light] : dir.each())
				draw(camera, transform.rposition, light.color, entity);

			auto point = registry.view<FTransformComponent, FPointLightComponent>();
			for (auto [entity, transform, light] : point.each())
				draw(camera, transform.rposition, light.color, entity);

			auto spot = registry.view<FTransformComponent, FSpotLightComponent>();
			for (auto [entity, transform, light] : spot.each())
				draw(camera, transform.rposition, light.color, entity);
		}
	}

	graphics->bindVertexBuffer(invalid_index);
	graphics->setManualShaderControlFlag(false);
	graphics->bindShader(invalid_index);
}

void CBillboardSystem::draw(FCameraComponent* camera, const glm::vec3& position, const glm::vec3& color, entt::entity entity)
{
	auto& pPush = graphics->getPushBlockHandle("billboard");

	auto needToDraw = camera->frustum.checkPoint(position);
	if (needToDraw)
	{
		pPush->set("billboardPosition", position);
		pPush->set("color", color);
		pPush->set("object_id", encodeIdToColor(static_cast<uint32_t>(entity)));
		graphics->flushConstantRanges(pPush);

		graphics->draw();
	}
}