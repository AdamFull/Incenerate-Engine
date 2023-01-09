#pragma once

namespace engine
{
	namespace editor
	{
		class CEditorWindow
		{
		public:
			virtual ~CEditorWindow() {}

			virtual void create() = 0;
			virtual void draw() = 0;
		protected:
			bool bIsOpen{ true };
		};
	}
}