#pragma once

namespace engine
{
	namespace editor
	{
		class CEditor
		{
		public:
			void create();

			void selectObject(const entt::entity& object);
			void deselectObject(const entt::entity& object);
			void deselectAll();

			bool isSelected(const entt::entity& object) const;

			const entt::entity& getLastSelection() const;

			vk::DescriptorPool& getDescriptorPool();
		private:
			entt::entity selected{ entt::null };
			vk::DescriptorPool descriptorPool{};
		};
	}
}