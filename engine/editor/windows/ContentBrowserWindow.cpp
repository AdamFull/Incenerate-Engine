#include "ContentBrowserWindow.h"

#include "Engine.h"

#include <imgui/imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include "system/filesystem/filesystem.h"

using namespace engine;
using namespace engine::system;
using namespace engine::editor;

constexpr const float padding{ 16.0f };
constexpr const float thumbnailSize{ 64.0f };
constexpr const float cellSize{ thumbnailSize + padding };

template<class _OnOkFunc>
void CreateFileModal(const char* label, _OnOkFunc&& onOkFunc)
{
	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, { 0.5f, 0.5f });

	if (ImGui::BeginPopupModal(label, nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
	{
		static std::string* text = new std::string;
		auto res = ImGui::InputText("##", text);

		ImGui::BeginDisabled(text->empty());
		if (ImGui::Button("OK"))
		{
			if (!text->empty())
				onOkFunc(*text);
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndDisabled();

		ImGui::SameLine();

		if (ImGui::Button("Cancel", { -1.f, 0.f }))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}

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

	// Top panel
	auto& upd = EGEditor->getIcon(icons::update);
	if (ImGui::Button(upd.c_str()))
		update_path(currentPath);
	ImGui::SameLine();

	if (ImGui::Button(nfdr.c_str()))
		open_popup_name = "Create directory";
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
		ImGui::Text(fs::from_unicode(relative).c_str());
	}

	ImGui::Separator();

	if (ImGui::BeginPopup("ContentBrowserSubmenu"))
	{
		if (ImGui::BeginMenu("create"))
		{
			if (ImGui::MenuItem((nfdr + " folder").c_str()))
				open_popup_name = "Create directory";

			auto& script = EGEditor->getIcon(icons::script_file);
			if (ImGui::MenuItem((script + " lua file").c_str()));
			if (ImGui::MenuItem("scene"))
				open_popup_name = "Create scene";

			ImGui::EndMenu();
		}

		auto& copy = EGEditor->getIcon(icons::copy);
		if (ImGui::MenuItem((copy + " copy").c_str(), "CTRL+C", false, !selected.empty()));

		auto& paste = EGEditor->getIcon(icons::paste);
		if (ImGui::MenuItem((paste + " paste").c_str(), "CTRL+V", false, false));

		auto& del = EGEditor->getIcon(icons::trash);
		if (ImGui::MenuItem((del + " delete").c_str(), "DEL", false, !selected.empty()))
			std::filesystem::remove_all(workdir / selected);

		ImGui::EndPopup();
	}

	CreateFileModal("Create scene",
		[this](const std::string& filename)
		{
			EGEditor->pushAction([this, filename]()
				{
					EGEditor->openExistingScene(currentPath / (filename + ".iescene"));
				});
			update_path(workdir);
		});

	CreateFileModal("Create directory",
		[this](const std::string& filename)
		{
			std::filesystem::create_directories(currentPath / filename);
			update_path(currentPath);
		});

	if (open_popup_name)
	{
		ImGui::OpenPopup(open_popup_name);
		open_popup_name = nullptr;
	}

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);
	if (columnCount < 1)
		columnCount = 1;

	if (ImGui::BeginTable("files", columnCount > 0 ? columnCount - 1 : columnCount))
	{
		int currentColumn{ 0 };
		auto pLargeIcons = EGEditor->getLargeIcons();

		bool bSelected{ false };
		for (auto& path : vCurrentDirData)
		{
			if (currentColumn < columnCount)
			{
				currentColumn++;
				ImGui::TableNextColumn();
			}
			else
				currentColumn = 0;

			auto relativePath = std::filesystem::relative(path, workdir);
			auto filename = fs::from_unicode(relativePath.filename().wstring());
			auto isDirectory = std::filesystem::is_directory(path);

			auto& icon = getFileIcon(filename, isDirectory);

			ImGui::PushFont(pLargeIcons);
			ImGui::PushID(filename.c_str());
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			auto double_clicked = ImGui::Button((icon + ("##" + filename)).c_str(), { thumbnailSize, thumbnailSize });
			ImGui::PopStyleColor();
			ImGui::PopFont();

			if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
			{
				if (ImGui::IsMouseClicked(1))
				{
					if (ImGui::IsItemHovered())
						selected = relativePath;
					else
						selected = "";

					open_popup_name = "ContentBrowserSubmenu";
				}
			}

			if (double_clicked && ImGui::IsItemHovered())
			{
				if (isDirectory)
				{
					auto npath = currentPath / path.filename();
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
				else if (fs::is_scene_format(path))
					EGEditor->pushAction([path]() { EGEditor->openExistingScene(path); });
			}

			if (!isDirectory && ImGui::BeginDragDropSource())
			{
				const wchar_t* itemPath = relativePath.c_str();
				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::Text((icon + (" " + fs::from_unicode(relativePath))).c_str());
				ImGui::EndDragDropSource();
			}

			ImGui::TextWrapped(filename.c_str());
			ImGui::NextColumn();
			ImGui::PopID();
		}

		ImGui::EndTable();
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
			auto path = entry.path();
			if (
				entry.is_directory() || 
				fs::is_image_format(path) || 
				fs::is_audio_format(path) || 
				fs::is_script_format(path) || 
				fs::is_mesh_format(path) ||
				fs::is_scene_format(path))
				vCurrentDirData.emplace_back(path);
		}
	}
}