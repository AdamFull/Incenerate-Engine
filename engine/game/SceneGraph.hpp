#pragma once

#include <string>
#include <list>
#include <stack>
#include <queue>

namespace engine
{
	namespace game
	{
		class scenegraph
		{
		public:
			static entt::entity create_node(const std::string& name);
			static void destroy_node(entt::entity node);
			static void attach_child(entt::entity parent, entt::entity child);
			static void detach_child(entt::entity parent, entt::entity child);
			static void detach_child(entt::entity child);
			static void parent_exchange(entt::entity new_parent, entt::entity child);
		};
	}
}