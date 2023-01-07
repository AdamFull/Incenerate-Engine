#include "SceneSerializer.h"

#include "Engine.h"

#include "system/filesystem/filesystem.h"

#include "ecs/components/AudioComponent.h"
#include "ecs/components/MeshComponent.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/components/CameraComponent.h"
#include "ecs/components/SpriteComponent.h"
#include "ecs/components/DirectionalLightComponent.h"
#include "ecs/components/PointLightComponent.h"
#include "ecs/components/SpotLightComponent.h"
#include "ecs/components/SkyboxComponent.h"
#include "ecs/components/ScriptComponent.h"

#include "loaders/MeshLoader.h"

#include "audio/AudioSource.h"

namespace engine
{
	namespace game
	{
		void to_json(nlohmann::json& json, const FSceneObjectRaw& type)
		{
			json = nlohmann::json
			{
				{"name", type.srName},
				{"components", type.mComponents},
				{"children", type.vChildren}
			};
		}

		void from_json(const nlohmann::json& json, FSceneObjectRaw& type)
		{
			utl::parse_to("name", json, type.srName);
			utl::parse_to("components", json, type.mComponents);
			utl::parse_to("children", json, type.vChildren);
		}
	}
}

using namespace engine::ecs;
using namespace engine::game;
using namespace engine::system;
using namespace engine::loaders;
using namespace engine::graphics;
using namespace engine::audio;

std::unique_ptr<CSceneNode> CSceneLoader::load(const std::string& scenepath)
{
	std::vector<FSceneObjectRaw> vScene;
	fs::read_json(scenepath, vScene);

	auto pRoot = std::make_unique<CSceneNode>("root");

	loadNodes(pRoot, vScene);

	return pRoot;
}

void CSceneLoader::save(const std::unique_ptr<CSceneNode>& pRoot, const std::string& scenepath)
{
	std::vector<FSceneObjectRaw> vScene;

	for (auto& child : pRoot->getChildren())
		serializeNodes(child, vScene);

	fs::write_json(scenepath, vScene, 2);
}

void CSceneLoader::loadNodes(const std::unique_ptr<CSceneNode>& pParent, const std::vector<FSceneObjectRaw>& vObjects)
{
	for (auto& object : vObjects)
	{
		auto pNode = std::make_unique<CSceneNode>(object.srName);
		auto entity = pNode->getEntity();
		
		for (auto& [name, component] : object.mComponents)
		{
			if (name == "transform")
			{
				auto& transform = EGCoordinator.get<FTransformComponent>(entity);
				transform = component.get<FTransformComponent>();
			}

			if (name == "camera")
				EGCoordinator.emplace<FCameraComponent>(entity, component.get<FCameraComponent>());

			if (name == "audio")
			{
				auto audio = component.get<FAudioComponent>();
				auto pAudio = std::make_unique<CAudioSource>(audio.source);
				audio.asource = EGAudio->addSource(pNode->getName(), std::move(pAudio));
				EGCoordinator.emplace<FAudioComponent>(entity, audio);
			}

			if (name == "skybox")
			{
				auto skybox = component.get<FSkyboxComponent>();
				skybox.origin = EGGraphics->addImage(pNode->getName(), skybox.source);
				skybox.vbo_id = EGGraphics->addVertexBuffer(pNode->getName());
				skybox.shader_id = EGGraphics->addShader(pNode->getName(), "skybox");

				auto& pVBO = EGGraphics->getVertexBuffer(skybox.vbo_id);
				pVBO->addPrimitive(std::make_unique<FCubePrimitive>());
				pVBO->create();

				EGCoordinator.emplace<FSkyboxComponent>(entity, skybox);
			}
				

			if (name == "sprite")
				EGCoordinator.emplace<FSpriteComponent>(entity, component.get<FSpriteComponent>());

			if (name == "gltfscene")
			{
				auto source = component.get<std::string>();
				CMeshLoader::load(source, pNode);
			}

			if (name == "directionallight")
				EGCoordinator.emplace<FDirectionalLightComponent>(entity, component.get<FDirectionalLightComponent>());

			if (name == "pointlight")
				EGCoordinator.emplace<FPointLightComponent>(entity, component.get<FPointLightComponent>());

			if (name == "spotlight")
				EGCoordinator.emplace<FSpotLightComponent>(entity, component.get<FSpotLightComponent>());

			if (name == "script")
				EGCoordinator.emplace<FScriptComponent>(entity, component.get<FScriptComponent>());
		}

		loadNodes(pNode, object.vChildren);
		
		pParent->attach(std::move(pNode));
	}
}

void CSceneLoader::serializeNodes(const std::unique_ptr<CSceneNode>& pParent, std::vector<FSceneObjectRaw>& vObjects)
{
	FSceneObjectRaw object;
	object.srName = pParent->getName();

	auto entity = pParent->getEntity();
	if (auto transform = EGCoordinator.try_get<FTransformComponent>(entity))
		object.mComponents.emplace("transform", nlohmann::json(*transform));
	if (auto camera = EGCoordinator.try_get<FCameraComponent>(entity))
		object.mComponents.emplace("camera", nlohmann::json(*camera));
	if (auto audio = EGCoordinator.try_get<FAudioComponent>(entity))
		object.mComponents.emplace("audio", nlohmann::json(*audio));
	if (auto skybox = EGCoordinator.try_get<FSkyboxComponent>(entity))
		object.mComponents.emplace("skybox", nlohmann::json(*skybox));
	if (auto sprite = EGCoordinator.try_get<FSpriteComponent>(entity))
		object.mComponents.emplace("sprite", nlohmann::json(*sprite));
	if (auto directionallight = EGCoordinator.try_get<FDirectionalLightComponent>(entity))
		object.mComponents.emplace("directionallight", nlohmann::json(*directionallight));
	if (auto pointlight = EGCoordinator.try_get<FPointLightComponent>(entity))
		object.mComponents.emplace("pointlight", nlohmann::json(*pointlight));
	if (auto spotlight = EGCoordinator.try_get<FSpotLightComponent>(entity))
		object.mComponents.emplace("spotlight", nlohmann::json(*spotlight));
	if (auto script = EGCoordinator.try_get<FScriptComponent>(entity))
		object.mComponents.emplace("script", nlohmann::json(*script));

	//TODO: serialize nodes here

	for (auto& child : pParent->getChildren())
		serializeNodes(child, object.vChildren);

	vObjects.emplace_back(object);
}