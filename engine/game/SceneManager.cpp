#include "SceneManager.h"

#include "Engine.h"

#include "ecs/components/TransformComponent.h"

#include "SceneGraph.hpp"
#include "SceneSerializer.h"

using namespace engine::game;

CSceneManager::~CSceneManager()
{
	if (root != entt::null)
		scenegraph::destroy_node(root);
}

bool CSceneManager::make_new(const std::filesystem::path& path)
{
	release();

	root = scenegraph::create_node("root");

	CSceneLoader::save(root, path);
	scenepath = path;

	return true;
}

bool CSceneManager::load(const std::filesystem::path& path)
{
	release();

	root = CSceneLoader::load(path);
	scenepath = path;

	return root != entt::null;
}

void CSceneManager::save()
{
	if (root != entt::null)
		CSceneLoader::save(root, scenepath);
}

void CSceneManager::release()
{
	auto& graphics = EGEngine->getGraphics();

	if (root != entt::null)
		scenegraph::destroy_node(root);

	graphics->forceReleaseResources();
}

const entt::entity& CSceneManager::getRoot()
{
	return root;
}

const std::filesystem::path& CSceneManager::getScenePath() const
{
	return scenepath;
}
