#pragma once

#include "Operation.h"

namespace engine
{
	namespace editor
	{
		class CCreateEntityOperation : public IOperation
		{
		public:
			virtual ~CCreateEntityOperation() override = default;
			CCreateEntityOperation(entt::entity entity, size_t component_id);

			void undo() override;
			void redo() override;

		private:
			entt::entity entity{ entt::null };
			entt::entity parent{ entt::null };
			size_t component_id{ invalid_index };
			std::string name{ "SceneNode" };
		};
	}
}