#pragma once

namespace engine
{
	namespace ecs
	{
		struct FMeshletLOD
		{
			size_t begin_index, index_count;
			size_t begin_vertex, vertex_count;
		};

		struct FMeshlet
		{
			std::array<FMeshletLOD, MAX_LEVEL_OF_DETAIL> levels_of_detail;
			//size_t begin_index, index_count;
			//size_t begin_vertex, vertex_count;
			size_t material{ invalid_index };

			bool bWasCulled{ false };

			struct FDimensions
			{
				glm::vec3 min = glm::vec3(FLT_MAX);
				glm::vec3 max = glm::vec3(-FLT_MAX);
				glm::vec3 size;
				glm::vec3 center;
				float radius;
			} dimensions;

			void setDimensions(glm::vec3 min, glm::vec3 max)
			{
				dimensions.min = min;
				dimensions.max = max;
				dimensions.size = max - min;
				dimensions.center = (min + max) / 2.0f;
				dimensions.radius = glm::distance(min, max) / 2.0f;
			}
		};

		struct FMeshInstance
		{
			glm::mat4 model{};
			glm::vec4 color{ 1.f };
		};

		struct FMeshComponent
		{
			size_t vbo_id{ invalid_index };
			std::vector<FMeshlet> vMeshlets;
			int32_t skin{ -1 };
			entt::entity head{ entt::null };
			size_t instanceCount{ 0ull };
			std::array<FMeshInstance, 512> vInstances{};
			bool loaded{ false };
		};
	}
}