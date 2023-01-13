#include "ContentBrowserWindow.h"

#include "Engine.h"

#include <imgui/imgui.h>

#include "system/filesystem/filesystem.h"

using namespace engine;
using namespace engine::system;
using namespace engine::editor;

constexpr const float padding{ 16.0f };
constexpr const float thumbnailSize{ 64.0f };
constexpr const float cellSize{ thumbnailSize + padding };

const std::string& getFileIcon(const std::string& filename, bool isDirectory)
{
	if (isDirectory)
		return EGEditor->getIcon(icons::folder);
	else if (fs::is_image_format(filename))
		return EGEditor->getIcon(icons::image_file);
	else if (fs::is_audio_format(filename))
		return EGEditor->getIcon(icons::audio_file);
	else if (fs::is_script_format(filename))
		return EGEditor->getIcon(icons::script_file);
	else if (fs::is_mesh_format(filename))
		return EGEditor->getIcon(icons::mesh_file);

	return EGEditor->getIcon(icons::file);
}

void CEditorContentBrowser::create()
{
	workdir = fs::get_workdir();
	update_path(workdir);
}

void CEditorContentBrowser::__draw(float fDt)
{
	if (currentPath != workdir)
	{
		auto& undo = EGEditor->getIcon(icons::undo);
		if (ImGui::Button(undo.c_str()))
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
		auto relativePath = std::filesystem::relative(path, workdir);
		auto filename = relativePath.filename().string();
		auto isDirectory = std::filesystem::is_directory(path);

		auto& icon = getFileIcon(filename, isDirectory);

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
				auto npath = currentPath / fs::get_filename(path);
				update_path(npath);
				ImGui::PopID();
				break;
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
	{
		auto path = entry.path().string();
		if(entry.is_directory() || fs::is_image_format(path) || fs::is_audio_format(path) || fs::is_script_format(path) || fs::is_mesh_format(path))
			vCurrentDirData.emplace_back(path);
	}
		
}