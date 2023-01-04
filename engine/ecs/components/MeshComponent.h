#pragma once

#include <vector>

namespace engine
{
	namespace ecs
	{
		struct FMeshlet
		{
			size_t begin_index, index_count;
			size_t begin_vertex, vertex_count;
			size_t vertex_buffer{ invalid_index };
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

		struct FMeshComponent
		{
			bool bIsSkybox{ false };
			std::vector<FMeshlet> vMeshlets;
		};
	}
}