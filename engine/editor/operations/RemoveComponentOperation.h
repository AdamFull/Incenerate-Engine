#pragma once

#include "Operation.h"
#include <any>

namespace engine
{
	namespace editor
	{
		class CRemoveComponentOperation : public IOperation
		{
		public:
			virtual ~CRemoveComponentOperation() override = default;
			CRemoveComponentOperation(entt::entity entity, size_t component_id);

			void undo() override;
			void redo() override;

		private:
			entt::entity target{ entt::null };
			size_t component_id{ invalid_index };
			std::any component;
		};
	}
}