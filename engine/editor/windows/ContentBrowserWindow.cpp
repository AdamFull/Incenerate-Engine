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
	EGEngine->addEventListener(Events::Editor::ProjectUpdated, this, &CEditorContentBrowser::OnProjectChanged);
	workdir = fs::get_workdir();
	update_path(workdir);
}

void CEditorContentBrowser::__draw(float fDt)
{
	auto& nfdr = EGEditor->getIcon(icons::add_folder);
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
	{
		if (ImGui::IsMouseClicked(1))
			ImGui::OpenPopup("ContentBrowserSubmenu");
	}

	if (ImGui::BeginPopup("ContentBrowserSubmenu"))
	{

		if (ImGui::BeginMenu("create"))
		{
			if (ImGui::MenuItem((nfdr + " folder").c_str())); // Requires dialog
			auto& script = EGEditor->getIcon(icons::script_file);
			if (ImGui::MenuItem((script + " lua file").c_str()));
			if (ImGui::MenuItem("scene"));
			ImGui::EndMenu();
		}

		auto& copy = EGEditor->getIcon(icons::copy);
		if (ImGui::MenuItem((copy + " copy").c_str(), "CTRL+C"));

		auto& paste = EGEditor->getIcon(icons::paste);
		if (ImGui::MenuItem((paste + " paste").c_str(), "CTRL+V"));

		auto& del = EGEditor->getIcon(icons::trash);
		if (ImGui::MenuItem((del + " delete").c_str(), "DEL"));

		ImGui::EndPopup();
	}

	
	if (ImGui::Button(nfdr.c_str()));
	ImGui::SameLine();

	auto& nfl = EGEditor->getIcon(icons::add_file);
	if (ImGui::Button(nfl.c_str()));

	if (currentPath != workdir)
	{
		ImGui::SameLine();
		auto& undo = EGEditor->getIcon(icons::undo);
		if (ImGui::Button(undo.c_str()))
			update_path(currentPath.parent_path());
		ImGui::SameLine();
		ImGui::Text(relative.string().c_str());
	}

	ImGui::Separator();

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
			else if (fs::is_image_format(path))
			{
				CEvent eevent(Events::Editor::OpenImageViewer);
				eevent.setParam(Events::Editor::OpenImageViewer, path);
				EGEngine->sendEvent(eevent);
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

void CEditorContentBrowser::OnProjectChanged(CEvent& event)
{
	workdir = fs::get_workdir();
	update_path(workdir);
}

void CEditorContentBrowser::update_path(const std::filesystem::path& npath)
{
	currentPath = npath;
	relative = std::filesystem::relative(currentPath, workdir);
	vCurrentDirData.clear();

	if (std::filesystem::exists(currentPath))
	{
		for (auto& entry : std::filesystem::directory_iterator(currentPath))
		{
			auto path = entry.path().string();
			if (entry.is_directory() || fs::is_image_format(path) || fs::is_audio_format(path) || fs::is_script_format(path) || fs::is_mesh_format(path))
				vCurrentDirData.emplace_back(path);
		}
	}
}