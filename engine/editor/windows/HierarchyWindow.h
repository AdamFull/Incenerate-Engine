#pragma once

#include "WindowBase.h"

namespace engine
{
	namespace editor
	{
		class CEditorHierarchy : public CEditorWindow
		{
		public:
			virtual ~CEditorHierarchy() override = default;

			void create() override;
			void draw() override;
		private:
			void buildHierarchy(const entt::entity& entity);
		};
	}
}