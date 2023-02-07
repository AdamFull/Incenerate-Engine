#pragma once

#include <vulkan/vulkan.hpp>

#include "ecs/components/fwd.h"

namespace engine
{
	namespace loaders
	{
		class CObjLoader
		{
		public:
			void load(const std::filesystem::path& source, const entt::entity& pRoot, ecs::FSceneComponent* component);
		private:
			size_t tryLoadTexture(const std::filesystem::path& path, vk::Format oformat);
			size_t loadTexture(const std::filesystem::path& path, bool overridef, vk::Format oformat);

			size_t vbo_id{ invalid_index };
			std::vector<size_t> vMaterials;
			std::filesystem::path fsParentPath{ "" };
			std::filesystem::path fsRelPath{ "" };
		};
	}
}