#include "Engine.h"

// Components
#include "ecs/components/components.h"

#include "ecs/helper.hpp"

#include "filesystem/vfs_helper.h"

#include "loaders/MeshLoader.h"
#include "game/SceneGraph.hpp"
#include "game/TerrainGenerator.h"

using namespace engine;
using namespace engine::filesystem;
using namespace engine::audio;
using namespace engine::graphics;
using namespace engine::loaders;
using namespace engine::game;
using namespace engine::ecs;

void destroy_hierarchy(entt::registry& reg, entt::entity entity)
{
	if (reg.try_get<FHierarchyComponent>(entity))
	{
		while (!reg.try_get<FHierarchyComponent>(entity)->children.empty())
		{
			auto child = reg.try_get<FHierarchyComponent>(entity)->children.front();
			scenegraph::destroy_node(child);
		}
	}
}

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
	}
}

void destroy_scene(entt::registry& reg, entt::entity entity)
{
	auto& scene = reg.get<FSceneComponent>(entity);

	if (scene.loaded)
	{
		if (reg.try_get<FHierarchyComponent>(entity))
		{
			while (!reg.try_get<FHierarchyComponent>(entity)->children.empty())
			{
				auto child = reg.try_get<FHierarchyComponent>(entity)->children.front();
				scenegraph::destroy_node(child);
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
		skybox.prefiltred = graphics->computePrefiltered(skybox.origin, 1024);
		skybox.vbo_id = graphics->addVertexBuffer(skybox.source);

		FShaderCreateInfo specials;
		specials.pipeline_stage = "deferred";
		specials.vertex_type = EVertexType::eNone;
		specials.cull_mode = vk::CullModeFlagBits::eBack;
		specials.front_face = vk::FrontFace::eCounterClockwise;
		specials.usages = 1;
		skybox.shader_id = graphics->addShader("skybox:skybox", specials);
		skybox.loaded = true;

		auto& pVBO = graphics->getVertexBuffer(skybox.vbo_id);
		pVBO->addPrimitive(std::make_unique<FCubePrimitive>());
		pVBO->setLoaded();
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
		audio.asource = EGAudio->addSource(audio.source, audio.source);
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

/*void construct_particle(entt::registry& reg, entt::entity entity)
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
}*/

void construct_terrain(entt::registry& reg, entt::entity entity)
{
	auto& terrain = reg.get<FTerrainComponent>(entity);

	if (!terrain.loaded /*&& !terrain.source.empty()*/)
	{
		auto loader = std::make_unique<CTerrainLoader>();
		loader->load(&terrain);
		terrain.loaded = true;
	}
}

void destroy_terrain(entt::registry& reg, entt::entity entity)
{
	auto& terrain = reg.get<FTerrainComponent>(entity);

	if (terrain.loaded)
	{
		auto& graphics = EGEngine->getGraphics();
		graphics->removeImage(terrain.heightmap_id);
		graphics->removeMaterial(terrain.material_id);
		graphics->removeVertexBuffer(terrain.vbo_id);
	}
}

FCameraComponent* CEngine::getActiveCamera()
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

	return nullptr;
}

void CEngine::initEntityComponentSystem()
{
	registry.on_destroy<FHierarchyComponent>().connect<&destroy_hierarchy>();

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

	registry.on_construct<FTerrainComponent>().connect<&construct_terrain>();
	registry.on_destroy<FTerrainComponent>().connect<&destroy_terrain>();
}