#pragma once

namespace engine
{
	namespace editor
	{
		class IEditorWindow
		{
		public:
			virtual ~IEditorWindow() {}

			virtual void create() = 0;
			virtual void draw(float fDt);

			const std::string& getName() const { return name; }
			const bool isOpen() const { return bIsOpen; }
			void toggleEnable() { bIsOpen = !bIsOpen; }
		protected:
			virtual void __draw(float fDt) = 0;

			std::string name;
			bool bIsOpen{ true };
		};
	}
}