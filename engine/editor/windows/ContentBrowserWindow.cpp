#include "ContentBrowserWindow.h"

#include "Engine.h"

#include <imgui/imgui.h>
#include <imgui/IconsFontAwesome6.h>

#include "system/filesystem/filesystem.h"

using namespace engine::system;
using namespace engine::editor;

constexpr const float padding{ 16.0f };
constexpr const float thumbnailSize{ 64.0f };
constexpr const float cellSize{ thumbnailSize + padding };

void CEditorContentBrowser::create()
{
	workdir = fs::get_workdir();
	update_path(workdir);
}

void CEditorContentBrowser::__draw()
{
	if (currentPath != workdir)
	{
		if (ImGui::Button("<-"))
			update_path(currentPath.parent_path());
		ImGui::SameLine();
	}

	ImGui::Text(relative.string().c_str());

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);
	if (columnCount < 1)
		columnCount = 1;

	ImGui::Columns(columnCount, 0, false);

	auto pLargeIcons = EGEditor->getLargeIcons();

	
	bool bSelected{ false };
	for (auto& path : vCurrentDirData)
	{
		auto ext = path.extension().string();
		auto relativePath = std::filesystem::relative(path, workdir);
		auto filename = relativePath.filename().string();
		auto isDirectory = std::filesystem::is_directory(path);

		const char* icon{ nullptr };
		if (isDirectory)
			icon = ICON_FA_FOLDER;
		else if (ext == ".ktx" || ext == ".ktx2")
			icon = ICON_FA_IMAGE;
		else if (ext == ".wav" || ext == ".ogg")
			icon = ICON_FA_FILE_AUDIO;
		else if (ext == ".lua")
			icon = ICON_FA_FILE_CODE;
		else
			icon = ICON_FA_FILE;

		ImGui::PushFont(pLargeIcons);
		ImGui::PushID(filename.c_str());
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto double_clicked = ImGui::Button((icon + ("##" + filename)).c_str(), {thumbnailSize, thumbnailSize});
		ImGui::PopStyleColor();
		ImGui::PopFont();

		if (double_clicked && ImGui::IsItemHovered())
		{
			if (isDirectory)
			{
				auto npath = currentPath / path.filename();
				update_path(npath);
			}
		}

		if (!isDirectory && ImGui::BeginDragDropSource())
		{
			const wchar_t* itemPath = relativePath.c_str();
			ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
			ImGui::Text((icon+(" "+relativePath.string())).c_str());
			ImGui::EndDragDropSource();
		}

		ImGui::TextWrapped(filename.c_str());
		ImGui::NextColumn();
		ImGui::PopID();
	}
}

void CEditorContentBrowser::update_path(const std::filesystem::path& npath)
{
	currentPath = npath;
	relative = std::filesystem::relative(currentPath, workdir);
	vCurrentDirData.clear();

	for (auto& entry : std::filesystem::directory_iterator(currentPath))
		vCurrentDirData.emplace_back(entry.path());
}