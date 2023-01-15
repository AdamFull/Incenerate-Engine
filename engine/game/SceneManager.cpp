#include "SceneManager.h"

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
	if (root != entt::null)
		scenegraph::destroy_node(root);

	root = scenegraph::create_node("root");

	CSceneLoader::save(root, path);
	scenepath = path;

	return true;
}

bool CSceneManager::load(const std::filesystem::path& path)
{
	if (root != entt::null)
		scenegraph::destroy_node(root);

	root = CSceneLoader::load(path);
	scenepath = path;

	return root != entt::null;
}

void CSceneManager::save()
{
	if (root != entt::null)
		CSceneLoader::save(root, scenepath);
}

const entt::entity& CSceneManager::getRoot()
{
	return root;
}
