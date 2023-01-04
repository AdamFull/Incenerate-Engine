#pragma once

#include <string>
#include <list>
#include <stack>
#include <queue>
#include "core/Type.hpp"

namespace engine
{
	namespace ecs
	{
		class CSceneNode
		{
			using child_container_t = std::list<std::unique_ptr<CSceneNode>>;
		public:
			CSceneNode();
			~CSceneNode();

			std::stack<Entity> relative(Entity id);

			void exchange(CSceneNode* other, Entity id);
			void exchange(CSceneNode* other, const std::string& name);
			void exchange(CSceneNode* other, CSceneNode* child);

			void attach(std::unique_ptr<CSceneNode>&& node);

			std::unique_ptr<CSceneNode> detach(const std::string& name);
			std::unique_ptr<CSceneNode> detach(Entity id);
			std::unique_ptr<CSceneNode> detach(child_container_t::const_iterator node);

			template<class _Pred>
			const std::unique_ptr<CSceneNode>& find(_Pred predicate, bool bDeep = false) const { return *__find(predicate, bDeep); }

			const std::unique_ptr<CSceneNode>& find(Entity id, bool bDeep = false) const;
			const std::unique_ptr<CSceneNode>& find(const std::string& name, bool bDeep = false) const;

			inline const Entity getEntity() const { return entity; }

			const std::string& getName() const { return srName; }
			void setName(const std::string& value) { srName = value; }

			const bool isVisible(Entity id) const;
			const bool isVisible() const { return bVisible; }
			void setVisible(bool value) { bVisible = value; }

			const bool isEnabled(Entity id) const;
			const bool isEnabled() const { return bEnable; }
			void setEnabled(bool value) { bEnable = value; }
		protected:
			template<class _Pred>
			child_container_t::const_iterator __find(_Pred predicate, bool bDeep = false) const
			{
				std::queue<const CSceneNode*> sNodes;
				sNodes.push(this);
				
				while (!sNodes.empty())
				{
					auto& cur = sNodes.front();

					auto found = std::find_if(cur->vChildren.cbegin(), cur->vChildren.cend(), predicate);

					if (found != cur->vChildren.end())
						return found;
					else
					{
						if (!bDeep)
							break;

						for (auto& child : cur->vChildren)
							sNodes.emplace(child.get());
						
						sNodes.pop();
					}
				}

				return vChildren.cend();
			}

		private:
			Entity entity;
			std::string srName;
			bool bVisible{ true };
			bool bEnable{ true };

			CSceneNode* pParent{ nullptr };
			child_container_t vChildren;
		};
	}
}