#pragma once

#include "Operation.h"
#include "EngineStructures.h"

namespace engine
{
	namespace editor
	{
		class CRemoveEntityOperation : public IOperation
		{
		public:
			virtual ~CRemoveEntityOperation() override = default;
			CRemoveEntityOperation(entt::entity entity);

			void undo() override;
			void redo() override;

		private:
			FSceneObjectRaw serialized;

			entt::entity target{ entt::null };
			entt::entity parent{ entt::null };
		};
	}
}