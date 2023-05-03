#pragma once

#include "Operation.h"

namespace engine
{
	namespace editor
	{
		class CDuplicateEntityOperation : public IOperation
		{
		public:
			virtual ~CDuplicateEntityOperation() override = default;
			CDuplicateEntityOperation(entt::entity parent, entt::entity entity);

			void undo() override;
			void redo() override;

		private:
			entt::entity target{ entt::null };
			entt::entity parent{ entt::null };
			entt::entity entity{ entt::null };
		};
	}
}