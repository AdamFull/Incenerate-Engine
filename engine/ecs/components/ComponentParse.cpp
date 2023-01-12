#include "AudioComponent.h"
#include "CameraComponent.h"
#include "MeshComponent.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "DirectionalLightComponent.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"
#include "SkyboxComponent.h"
#include "ScriptComponent.h"
#include "SceneComponent.h"
#include "HierarchyComponent.h"

#include "Engine.h"
#include "game/SceneGraph.hpp"

#include "loaders/MeshLoader.h"

#include "parse/glmparse.h"

namespace engine
{
	namespace ecs
	{
		void to_json(nlohmann::json& json, const FAudioComponent& type)
		{
			json = nlohmann::json{};
			utl::serialize_from("source", json, type.source, !type.source.empty());
			utl::serialize_from("gain", json, type.gain, type.gain != 1.f);
			utl::serialize_from("pitch", json, type.pitch, type.pitch != 1.f);
			utl::serialize_from("loop", json, type.loop, type.loop);
		}

		void from_json(const nlohmann::json& json, FAudioComponent& type)
		{
			utl::parse_to("source", json, type.source);
			utl::parse_to("gain", json, type.gain);
			utl::parse_to("pitch", json, type.pitch);
			utl::parse_to("velocity", json, type.velocity);
			utl::parse_to("loop", json, type.loop);
		}


		void to_json(nlohmann::json& json, const FCameraComponent& type)
		{
			json = nlohmann::json{};
			utl::serialize_from("fov", json, type.fieldOfView, type.fieldOfView != 45.f);
			utl::serialize_from("near", json, type.nearPlane, type.nearPlane != 0.1f);
			utl::serialize_from("far", json, type.farPlane, type.farPlane != 128.f);
			utl::serialize_from("active", json, type.active, type.active);
		}

		void from_json(const nlohmann::json& json, FCameraComponent& type)
		{
			utl::parse_to("fov", json, type.fieldOfView);
			utl::parse_to("near", json, type.nearPlane);
			utl::parse_to("far", json, type.farPlane);
			utl::parse_to("active", json, type.active);
		}


		void to_json(nlohmann::json& json, const FTransformComponent& type)
		{
			json = nlohmann::json{};
			utl::serialize_from("pos", json, type.position, type.position != glm::vec3(0.f));
			utl::serialize_from("rot", json, type.rotation, type.rotation != glm::vec3(0.f));
			utl::serialize_from("scale", json, type.scale, type.scale != glm::vec3(1.f));
		}
		
		void from_json(const nlohmann::json& json, FTransformComponent& type)
		{
			utl::parse_to("pos", json, type.position);
			utl::parse_to("rot", json, type.rotation);
			utl::parse_to("scale", json, type.scale);
		}


		void to_json(nlohmann::json& json, const FDirectionalLightComponent& type)
		{
			json = nlohmann::json{};
			utl::serialize_from("color", json, type.color, type.color != glm::vec3(0.f));
			utl::serialize_from("intencity", json, type.intencity, type.intencity != 1.f);
			utl::serialize_from("castShadows", json, type.castShadows, type.castShadows);
		}

		void from_json(const nlohmann::json& json, FDirectionalLightComponent& type)
		{
			utl::parse_to("color", json, type.color);
			utl::parse_to("intencity", json, type.intencity);
			utl::parse_to("castShadows", json, type.castShadows);
		}


		void to_json(nlohmann::json& json, const FPointLightComponent& type)
		{
			json = nlohmann::json{};
			utl::serialize_from("color", json, type.color, type.color != glm::vec3(0.f));
			utl::serialize_from("intencity", json, type.intencity, type.intencity != 1.f);
			utl::serialize_from("radius", json, type.radius, type.radius != 1.f);
			utl::serialize_from("castShadows", json, type.castShadows, type.castShadows);
		}

		void from_json(const nlohmann::json& json, FPointLightComponent& type)
		{
			utl::parse_to("color", json, type.color);
			utl::parse_to("intencity", json, type.intencity);
			utl::parse_to("radius", json, type.radius);
			utl::parse_to("castShadows", json, type.castShadows);
		}


		void to_json(nlohmann::json& json, const FSpotLightComponent& type)
		{
			json = nlohmann::json{};
			utl::serialize_from("color", json, type.color, type.color != glm::vec3(0.f));
			utl::serialize_from("intencity", json, type.intencity, type.intencity != 1.f);
			utl::serialize_from("innerAngle", json, type.innerAngle, type.innerAngle != 0.92f);
			utl::serialize_from("outerAngle", json, type.outerAngle, type.outerAngle != 0.98f);
			utl::serialize_from("toTarget", json, type.toTarget, type.toTarget);
			utl::serialize_from("castShadows", json, type.castShadows, type.castShadows);
		}

		void from_json(const nlohmann::json& json, FSpotLightComponent& type)
		{
			utl::parse_to("color", json, type.color);
			utl::parse_to("intencity", json, type.intencity);
			utl::parse_to("innerAngle", json, type.innerAngle);
			utl::parse_to("outerAngle", json, type.outerAngle);
			utl::parse_to("toTarget", json, type.toTarget);
			utl::parse_to("castShadows", json, type.castShadows);
		}


		void to_json(nlohmann::json& json, const FSpriteComponent& type)
		{
			json = nlohmann::json{};
			utl::serialize_from("source", json, type.source, !type.source.empty());
		}

		void from_json(const nlohmann::json& json, FSpriteComponent& type)
		{
			utl::parse_to("source", json, type.source);
		}


		void to_json(nlohmann::json& json, const FScriptComponent& type)
		{
			json = nlohmann::json{};
			utl::serialize_from("source", json, type.source, !type.source.empty());
		}

		void from_json(const nlohmann::json& json, FScriptComponent& type)
		{
			utl::parse_to("source", json, type.source);
		}


		void to_json(nlohmann::json& json, const FSkyboxComponent& type)
		{
			json = nlohmann::json{};
			utl::serialize_from("source", json, type.source, !type.source.empty());
		}

		void from_json(const nlohmann::json& json, FSkyboxComponent& type)
		{
			utl::parse_to("source", json, type.source);
		}


		void to_json(nlohmann::json& json, const FSceneComponent& type)
		{
			json = nlohmann::json{};
			utl::serialize_from("source", json, type.source, !type.source.empty());
		}

		void from_json(const nlohmann::json& json, FSceneComponent& type)
		{
			utl::parse_to("source", json, type.source);
		}
	}
}

using namespace engine::loaders;
using namespace engine::graphics;
using namespace engine::audio;
using namespace engine::game;
using namespace engine::ecs;

FAudioComponent::~FAudioComponent()
{
	if(loaded)
		EGAudio->removeSource(asource);
}

void FAudioComponent::create()
{
	auto pAudio = std::make_unique<CAudioSource>(source);
	asource = EGAudio->addSource(source, std::move(pAudio));
	loaded = true;
}

FMeshComponent::~FMeshComponent()
{
	if (loaded)
	{
		EGGraphics->removeVertexBuffer(vbo_id);

		for (auto& meshlet : vMeshlets)
			EGGraphics->removeMaterial(meshlet.material);
	}
}

FSkyboxComponent::~FSkyboxComponent()
{
	if (loaded)
	{
		EGGraphics->removeVertexBuffer(vbo_id);
		EGGraphics->removeShader(shader_id);
		EGGraphics->removeImage(origin);
		EGGraphics->removeImage(prefiltred);
		EGGraphics->removeImage(irradiance);
	}
}

void FSkyboxComponent::create()
{
	origin = EGGraphics->addImage(source, source);
	irradiance = EGGraphics->computeIrradiance(origin, 64);
	prefiltred = EGGraphics->computePrefiltered(origin, 512);
	vbo_id = EGGraphics->addVertexBuffer(source);
	shader_id = EGGraphics->addShader(source, "skybox");
	loaded = true;

	auto& pVBO = EGGraphics->getVertexBuffer(vbo_id);
	pVBO->addPrimitive(std::make_unique<FCubePrimitive>());
	pVBO->create();
}

FSceneComponent::~FSceneComponent()
{
	auto& registry = EGCoordinator;

	if (loaded)
	{
		auto& hierarchy = registry.get<FHierarchyComponent>(self);
		for (auto& entity : hierarchy.children)
			scenegraph::destroy_node(entity);
	}
}

void FSceneComponent::create(entt::entity node)
{
	CMeshLoader::load(source, node);
	self = node;
	loaded = true;
}