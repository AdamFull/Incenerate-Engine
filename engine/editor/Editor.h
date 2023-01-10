#pragma once

#include "windows/WindowBase.h"

namespace engine
{
	namespace editor
	{
		class CEditor
		{
		public:
			~CEditor();
			void create();
			void newFrame();

			void selectObject(const entt::entity& object);
			void deselectObject(const entt::entity& object);
			void deselectAll();

			bool isSelected(const entt::entity& object) const;

			const entt::entity& getLastSelection() const;

			vk::DescriptorPool& getDescriptorPool();
		private:
			void baseInitialize();
		private:
			entt::entity selected{ entt::null };
			std::vector<std::unique_ptr<IEditorWindow>> vEditorWindows;
			vk::DescriptorPool descriptorPool{};
		};
	}
}