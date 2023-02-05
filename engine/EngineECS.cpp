#include "Engine.h"

// Components
#include "ecs/components/components.h"

// Systems
#include "ecs/systems/systems.h"

#include "ecs/helper.hpp"

#include "system/filesystem/filesystem.h"

#include "loaders/MeshLoader.h"
#include "game/SceneGraph.hpp"

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
		auto& graphics = EGEngine->getGraphics();
		graphics->removeVertexBuffer(mesh.vbo_id);

		for (auto& meshlet : mesh.vMeshlets)
			graphics->removeMaterial(meshlet.material);
	}
}

void construct_rigidbody(entt::registry& reg, entt::entity entity)
{
	auto& rigidbody = reg.get<FRigidBodyComponent>(entity);

	if (rigidbody.object_id == invalid_index)
	{
		auto& physics = EGEngine->getPhysics();

		rigidbody.object_id = physics->addRigidBody(std::to_string(static_cast<uint32_t>(entity)));

		auto& object = physics->getObject(rigidbody.object_id);
		object->initialize(&rigidbody);
	}
}

void destroy_rigidbody(entt::registry& reg, entt::entity entity)
{
	auto& rigidbody = reg.get<FRigidBodyComponent>(entity);

	if (rigidbody.object_id != invalid_index)
	{
		auto& physics = EGEngine->getPhysics();
		physics->removeObject(rigidbody.object_id);
	}
}

void construct_scene(entt::registry& reg, entt::entity entity)
{
	auto& scene = reg.get<FSceneComponent>(entity);

	if (!scene.loaded && !scene.source.empty())
	{
		CMeshLoader::load(scene.source, entity, &scene);
		scene.loaded = true;

		auto& hierarchy = reg.get<FHierarchyComponent>(entity);
		int iii = 0;
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
	auto& graphics = EGEngine->getGraphics();
	auto& skybox = reg.get<FEnvironmentComponent>(entity);

	if (!skybox.loaded && !skybox.source.empty())
	{
		skybox.origin = graphics->addImage(fs::get_filename(skybox.source), skybox.source);
		skybox.irradiance = graphics->computeIrradiance(skybox.origin, 64);
		skybox.prefiltred = graphics->computePrefiltered(skybox.origin, 512);
		skybox.vbo_id = graphics->addVertexBuffer(skybox.source);
		skybox.shader_id = graphics->addShader(skybox.source, "skybox");
		skybox.loaded = true;

		auto& pVBO = graphics->getVertexBuffer(skybox.vbo_id);
		pVBO->addPrimitive(std::make_unique<FCubePrimitive>());
		pVBO->create();
		set_active_skybox(reg, entity);
	}
}

void destroy_skybox(entt::registry& reg, entt::entity entity)
{
	auto& graphics = EGEngine->getGraphics();
	auto& skybox = reg.get<FEnvironmentComponent>(entity);

	if (skybox.loaded)
	{
		graphics->removeVertexBuffer(skybox.vbo_id);
		graphics->removeShader(skybox.shader_id);
		graphics->removeImage(skybox.origin);
		graphics->removeImage(skybox.prefiltred);
		graphics->removeImage(skybox.irradiance);
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


void construct_script(entt::registry& reg, entt::entity entity)
{
	auto& script = reg.get<FScriptComponent>(entity);

	if (!script.loaded && !script.source.empty())
	{
		script.data = EGScripting->addSource(script.source, script.source);
		script.loaded = true;
	}
}

void destroy_script(entt::registry& reg, entt::entity entity)
{
	auto& script = reg.get<FScriptComponent>(entity);

	if (script.loaded)
		EGScripting->removeSource(script.data);
}

void construct_particle(entt::registry& reg, entt::entity entity)
{
	auto& particle = reg.get<FParticleComponent>(entity);

	if (!particle.loaded && !particle.source.empty())
	{
		auto& particles = EGEngine->getParticles();
		particle.particle_id = particles->addParticle(particle.source);
		
		auto& pObject = particles->getParticle(particle.particle_id);

		particle.loaded = true;
	}
}

void destroy_particle(entt::registry& reg, entt::entity entity)
{
	auto& particle = reg.get<FParticleComponent>(entity);

	if (particle.loaded)
	{
		auto& particles = EGEngine->getParticles();
		particles->removeParticle(particle.particle_id);
	}
}

FCameraComponent* CEngine::getActiveCamera()
{
	if (isEditorEditing())
		return registry.try_get<FCameraComponent>(pEditor->getCamera());
	else
	{
		entt::entity found{ entt::null };
		auto view = registry.view<FCameraComponent>();
		for (auto [entity, camera] : view.each())
		{
			if (camera.active)
			{
				found = entity;
				break;
			}
		}

		if(found != entt::null)
			return registry.try_get<FCameraComponent>(found);
	}

	return nullptr;
}

void CEngine::initEntityComponentSystem()
{
	registry.on_destroy<FMeshComponent>().connect<&destroy_mesh>();

	registry.on_construct<FRigidBodyComponent>().connect<&construct_rigidbody>();
	registry.on_destroy<FRigidBodyComponent>().connect<&destroy_rigidbody>();

	registry.on_construct<FSceneComponent>().connect<&construct_scene>();
	registry.on_destroy<FSceneComponent>().connect<&destroy_scene>();

	registry.on_construct<FEnvironmentComponent>().connect<&construct_skybox>();
	registry.on_destroy<FEnvironmentComponent>().connect<&destroy_skybox>();

	registry.on_construct<FAudioComponent>().connect<&construct_audio>();
	registry.on_destroy<FAudioComponent>().connect<&destroy_audio>();

	registry.on_construct<FScriptComponent>().connect<&construct_script>();
	registry.on_destroy<FScriptComponent>().connect<&destroy_script>();

	registry.on_construct<FParticleComponent>().connect<&construct_particle>();
	registry.on_destroy<FParticleComponent>().connect<&destroy_particle>();

	vSystems.emplace_back(std::make_unique<CAnimationSystem>());
	vSystems.emplace_back(std::make_unique<CHierarchySystem>());
	vSystems.emplace_back(std::make_unique<CPhysicsSystem>());
	vSystems.emplace_back(std::make_unique<CParticlesSystem>());

	vSystems.emplace_back(std::make_unique<CInputSystem>());
	vSystems.emplace_back(std::make_unique<CScriptingSystem>());
	vSystems.emplace_back(std::make_unique<CCameraControlSystem>());
	vSystems.emplace_back(std::make_unique<CAudioSystem>());
	vSystems.emplace_back(std::make_unique<C3DRenderSystem>());
	vSystems.emplace_back(std::make_unique<CPresentRenderSystem>());
}