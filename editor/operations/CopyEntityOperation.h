#pragma once

#include "Operation.h"

namespace engine
{
	namespace editor
	{
		class CCopyEntityOperation : public IOperation
		{
		public:
			virtual ~CCopyEntityOperation() override = default;
			CCopyEntityOperation(entt::entity parent, entt::entity entity);

			void undo() override;
			void redo() override;

		private:
			entt::entity target{ entt::null };
			entt::entity parent{ entt::null };
			entt::entity entity{ entt::null };
			std::string name{ "SceneNode" };
		};
	}
}