#include "Editor.h"

#include <entt/entt.hpp>

using namespace engine::editor;

void CEditor::create()
{

}

void CEditor::selectObject(const entt::entity& object)
{
	selected = object;
}

void CEditor::deselectObject(const entt::entity& object)
{
	selected = entt::null;
}

void CEditor::deselectAll()
{
	selected = entt::null;
}

bool CEditor::isSelected(const entt::entity& object) const
{
	return selected == object;
}

const entt::entity& CEditor::getLastSelection() const
{
	return selected;
}

vk::DescriptorPool& CEditor::getDescriptorPool()
{
	return descriptorPool;
}
