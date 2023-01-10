#include "WindowBase.h"

#include <imgui/imgui.h>

using namespace engine::editor;

void IEditorWindow::draw()
{
	if (bIsOpen)
	{
		if (ImGui::Begin(name.c_str(), &bIsOpen))
		{
			__draw();
			ImGui::End();
		}
	}
}