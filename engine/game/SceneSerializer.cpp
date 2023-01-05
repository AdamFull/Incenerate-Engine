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
				auto& transform = EGCoordinator->getComponent<FTransformComponent>(entity);
				transform = component.get<FTransformComponent>();
			}

			if (name == "camera")
				EGCoordinator->addComponent(entity, component.get<FCameraComponent>());

			if (name == "audio")
				EGCoordinator->addComponent(entity, component.get<FAudioComponent>());

			if (name == "skybox")
				EGCoordinator->addComponent(entity, component.get<FSkyboxComponent>());

			if (name == "sprite")
				EGCoordinator->addComponent(entity, component.get<FSpriteComponent>());

			if (name == "gltfscene")
			{
				auto source = component.get<std::string>();
				CMeshLoader::load(source, pNode);
			}

			if (name == "directionallight")
				EGCoordinator->addComponent(entity, component.get<FDirectionalLightComponent>());

			if (name == "pointlight")
				EGCoordinator->addComponent(entity, component.get<FPointLightComponent>());

			if (name == "spotlight")
				EGCoordinator->addComponent(entity, component.get<FSpotLightComponent>());

			if (name == "script")
				EGCoordinator->addComponent(entity, component.get<FScriptComponent>());
		}

		loadNodes(pNode, object.vChildren);
		
		pParent->attach(std::move(pNode));
	}
}

void CSceneLoader::serializeNodes(const std::unique_ptr<CSceneNode>& pParent, std::vector<FSceneObjectRaw>& vObjects)
{
	FSceneObjectRaw object;
	object.srName = pParent->getName();

	//TODO: serialize nodes here

	for (auto& child : pParent->getChildren())
		serializeNodes(child, object.vChildren);
}