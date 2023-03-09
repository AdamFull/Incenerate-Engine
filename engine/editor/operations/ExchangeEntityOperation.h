#pragma once

#include "Operation.h"
#include <any>

namespace engine
{
	namespace editor
	{
		class CExchangeEntityOperation : public IOperation
		{
		public:
			virtual ~CExchangeEntityOperation() override = default;
			CExchangeEntityOperation(entt::entity target, entt::entity new_parent);

			void undo() override;
			void redo() override;
		private:
			entt::entity target{ entt::null };
			entt::entity new_parent{ entt::null };
		};
	}
}