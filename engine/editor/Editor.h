#pragma once

#include "windows/WindowBase.h"

struct ImFont;

namespace engine
{
	namespace editor
	{
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

			ImFont* getLargeIcons() { return pLargeIcons; }
		private:
			void baseInitialize();
		private:
			entt::entity selected{ entt::null };

			ImFont* pSmallIcons{ nullptr };
			ImFont* pLargeIcons{ nullptr };

			std::vector<std::unique_ptr<IEditorWindow>> vEditorWindows;
			vk::DescriptorPool descriptorPool{};
		};
	}
}