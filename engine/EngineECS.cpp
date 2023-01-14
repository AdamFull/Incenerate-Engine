#include "Engine.h"

#include <utility/uthreading.hpp>

// Components
#include "ecs/components/AudioComponent.h"
#include "ecs/components/MeshComponent.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/components/HierarchyComponent.h"
#include "ecs/components/CameraComponent.h"
#include "ecs/components/SpriteComponent.h"
#include "ecs/components/DirectionalLightComponent.h"
#include "ecs/components/PointLightComponent.h"
#include "ecs/components/SpotLightComponent.h"
#include "ecs/components/EnvironmentComponent.h"
#include "ecs/components/ScriptComponent.h"
#include "ecs/components/SceneComponent.h"

// Systems
#include "ecs/systems/HierarchySystem.h"
#include "ecs/systems/AudioSystem.h"
#include "ecs/systems/rendering/3DRenderSystem.h"
#include "ecs/systems/rendering/2DRenderSystem.h"
#include "ecs/systems/rendering/EditorRenderSystem.h"
#include "ecs/systems/CameraControlSystem.h"
#include "ecs/systems/InputSystem.h"
#include "ecs/systems/ScriptingSystem.h"

#include "ecs/helper.hpp"

#include "system/filesystem/filesystem.h"

#include "loaders/MeshLoader.h"

using namespace engine;
using namespace engine::system;
using namespace engine::audio;
using namespace engine::graphics;
using namespace engine::loaders;
using namespace engine::game;
using namespace engine::ecs;

void destroy_mesh(entt::registry& reg, entt::entity entity)
{
	auto& mesh = reg.get<FMeshComponent>(entity);

	if (mesh.loaded)
	{
		EGGraphics->removeVertexBuffer(mesh.vbo_id);

		for (auto& meshlet : mesh.vMeshlets)
			EGGraphics->removeMaterial(meshlet.material);
	}
}

void construct_scene(entt::registry& reg, entt::entity entity)
{
	auto& scene = reg.get<FSceneComponent>(entity);

	if (!scene.loaded && !scene.source.empty())
	{
		CMeshLoader::load(scene.source, entity);
		scene.loaded = true;
	}
}

void destroy_scene(entt::registry& reg, entt::entity entity)
{
	auto& scene = reg.get<FSceneComponent>(entity);

	if (scene.loaded)
	{
		if (auto* hierarchy = reg.try_get<FHierarchyComponent>(entity))
		{
			while (!hierarchy->children.empty())
			{
				auto entity = hierarchy->children.front();
				scenegraph::destroy_node(entity);
			}
		}
	}
}


void construct_skybox(entt::registry& reg, entt::entity entity)
{
	auto& skybox = reg.get<FEnvironmentComponent>(entity);

	if (!skybox.loaded && !skybox.source.empty())
	{
		skybox.origin = EGGraphics->addImage(fs::get_filename(skybox.source), skybox.source);
		skybox.irradiance = EGGraphics->computeIrradiance(skybox.origin, 64);
		skybox.prefiltred = EGGraphics->computePrefiltered(skybox.origin, 512);
		skybox.vbo_id = EGGraphics->addVertexBuffer(skybox.source);
		skybox.shader_id = EGGraphics->addShader(skybox.source, "skybox");
		skybox.loaded = true;

		auto& pVBO = EGGraphics->getVertexBuffer(skybox.vbo_id);
		pVBO->addPrimitive(std::make_unique<FCubePrimitive>());
		pVBO->create();
		set_active_skybox(reg, entity);
	}
}

void destroy_skybox(entt::registry& reg, entt::entity entity)
{
	auto& skybox = reg.get<FEnvironmentComponent>(entity);

	if (skybox.loaded)
	{
		EGGraphics->removeVertexBuffer(skybox.vbo_id);
		EGGraphics->removeShader(skybox.shader_id);
		EGGraphics->removeImage(skybox.origin);
		EGGraphics->removeImage(skybox.prefiltred);
		EGGraphics->removeImage(skybox.irradiance);
	}
}

void construct_audio(entt::registry& reg, entt::entity entity)
{
	auto& audio = reg.get<FAudioComponent>(entity);

	if (!audio.loaded && !audio.source.empty())
	{
		auto pAudio = std::make_unique<CAudioSource>(audio.source);
		audio.asource = EGAudio->addSource(audio.source, std::move(pAudio));
		audio.loaded = true;
	}
}

void destroy_audio(entt::registry& reg, entt::entity entity)
{
	auto& audio = reg.get<FAudioComponent>(entity);

	if (audio.loaded)
		EGAudio->removeSource(audio.asource);
}

void CEngine::initEntityComponentSystem()
{
	registry.on_destroy<FMeshComponent>().connect<&destroy_mesh>();

	registry.on_construct<FSceneComponent>().connect<&construct_scene>();
	registry.on_destroy<FSceneComponent>().connect<&destroy_scene>();

	registry.on_construct<FEnvironmentComponent>().connect<&construct_skybox>();
	registry.on_destroy<FEnvironmentComponent>().connect<&destroy_skybox>();

	registry.on_construct<FAudioComponent>().connect<&construct_audio>();
	registry.on_destroy<FAudioComponent>().connect<&destroy_audio>();

	vSystems.emplace_back(std::make_unique<CHierarchySystem>());
	vSystems.emplace_back(std::make_unique<CInputSystem>());
	vSystems.emplace_back(std::make_unique<CScriptingSystem>());
	vSystems.emplace_back(std::make_unique<CCameraControlSystem>());
	vSystems.emplace_back(std::make_unique<CAudioSystem>());
	vSystems.emplace_back(std::make_unique<C3DRenderSystem>());

	if (bEditorMode)
		vSystems.emplace_back(std::make_unique<CEditorRenderSystem>());


	//vSystems.emplace_back(std::make_unique<C2DRenderSystem>());
}