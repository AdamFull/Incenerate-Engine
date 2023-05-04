#pragma once

namespace engine
{
	namespace ecs
	{
		struct FHierarchyComponent
		{
			FHierarchyComponent() = default;
			FHierarchyComponent(const std::string& name) : name(name) {}

			std::string name;
			entt::entity parent{ entt::null };
			std::vector<entt::entity> children;
			bool hidden{ false };
		};
	}
}