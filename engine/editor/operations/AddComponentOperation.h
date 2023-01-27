#pragma once

#include "Operation.h"

namespace engine
{
	namespace editor
	{
		class CAddComponentOperation : public IOperation
		{
		public:
			virtual ~CAddComponentOperation() override = default;
			CAddComponentOperation(entt::entity entity, size_t component_id);

			void undo() override;
			void redo() override;

		private:
			entt::entity target{ entt::null };
			size_t component_id{ invalid_index };
		};
	}
}