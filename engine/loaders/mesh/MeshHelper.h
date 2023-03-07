#pragma once

#include "graphics/APIStructures.h"

namespace engine
{
	namespace loaders
	{
        void calculate_normals(std::vector<graphics::FVertex>& vertices, const std::vector<uint32_t>& indices, uint64_t startIndex);
        void calculate_normals_quads(std::vector<graphics::FVertex>& vertices, const std::vector<uint32_t>& indices, uint64_t startIndex);
		void calculate_tangents(std::vector<graphics::FVertex>& vertices, const std::vector<uint32_t>& indices, uint64_t startIndex);
	}
}