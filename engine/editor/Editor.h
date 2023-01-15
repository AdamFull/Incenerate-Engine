#pragma once

#include "windows/WindowBase.h"
#include <utility/upattern.hpp>

#include "EditorProject.h"

struct ImFont;

namespace engine
{
	namespace editor
	{
		namespace icons
		{
			using namespace utl;

			constexpr uint32_t trash = "trash"_utl_hash;
			constexpr uint32_t play = "play"_utl_hash;
			constexpr uint32_t pause = "pause"_utl_hash;
			constexpr uint32_t stop = "stop"_utl_hash;
			constexpr uint32_t undo = "undo"_utl_hash;
			constexpr uint32_t redo = "redo"_utl_hash;
			constexpr uint32_t cut = "cut"_utl_hash;
			constexpr uint32_t copy = "copy"_utl_hash;
			constexpr uint32_t paste = "paste"_utl_hash;
			constexpr uint32_t save = "save"_utl_hash;
			constexpr uint32_t save_all = "save_all"_utl_hash;
			constexpr uint32_t close = "close"_utl_hash;
			constexpr uint32_t plus = "plus"_utl_hash;
			constexpr uint32_t node = "node"_utl_hash;

			constexpr uint32_t file_copy = "file_copy"_utl_hash;
			constexpr uint32_t file_paste = "file_paste"_utl_hash;

			constexpr uint32_t folder = "folder"_utl_hash;
			constexpr uint32_t file = "file"_utl_hash;
			constexpr uint32_t image_file = "image_file"_utl_hash;
			constexpr uint32_t audio_file = "audio_file"_utl_hash;
			constexpr uint32_t script_file = "script_file"_utl_hash;
			constexpr uint32_t mesh_file = "mesh_file"_utl_hash;
			constexpr uint32_t add_folder = "add_folder"_utl_hash;
			constexpr uint32_t add_file = "add_file"_utl_hash;
		}

		struct FRecentProjects
		{
			std::string recent;
		};

		void to_json(nlohmann::json& json, const FRecentProjects& type);
		void from_json(const nlohmann::json& json, FRecentProjects& type);

		class CEditor
		{
		public:
			~CEditor();
			void create();
			void newFrame(float fDt);

			void selectObject(const entt::entity& object);
			void deselectObject(const entt::entity& object);
			void deselectAll();

			bool isSelected(const entt::entity& object) const;

			const entt::entity& getLastSelection() const;

			vk::DescriptorPool& getDescriptorPool();

			template<class _Ty>
			const std::string& getIcon() { return getIcon(get_class_id<_Ty>()); }
			const std::string& getIcon(uint32_t id);

			const entt::entity& getCamera() const { return camera; }

			ImFont* getLargeIcons() { return pLargeIcons; }
		private:
			void NewProjectModal();
			void OpenProjectModal();
			void load_editor();
			void save_editor();

			template<class _Ty>
			static uint32_t get_class_id()
			{
				return utl::const_hash(typeid(_Ty).name());
			}

			void baseInitialize();
		private:
			FRecentProjects recproj;
			std::unique_ptr<CEditorProject> pEditorProject;

			entt::entity camera{ entt::null };
			entt::entity selected{ entt::null };
			std::map<uint32_t, std::string> mEditorIcons;

			ImFont* pSmallIcons{ nullptr };
			ImFont* pLargeIcons{ nullptr };

			std::vector<std::unique_ptr<IEditorWindow>> vEditorWindows;
			vk::DescriptorPool descriptorPool{};
		};
	}
}