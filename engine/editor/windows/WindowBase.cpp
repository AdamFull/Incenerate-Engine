#include "WindowBase.h"

#include <imgui/imgui.h>

using namespace engine::editor;

void IEditorWindow::draw(float fDt)
{
	if (bIsOpen)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.f, 0.f));
		if (ImGui::Begin(name.c_str(), &bIsOpen))
		{
			__draw(fDt);
			ImGui::End();
		}
		ImGui::PopStyleVar();
	}
}