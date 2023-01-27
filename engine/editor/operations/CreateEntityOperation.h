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
			CCreateEntityOperation(entt::entity entity);

			void undo() override;
			void redo() override;

		private:
			entt::entity entity{ entt::null };
			entt::entity parent{ entt::null };
			std::string name{ "SceneNode" };
		};
	}
}