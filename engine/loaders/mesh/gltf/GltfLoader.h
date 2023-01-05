#pragma once

#include "external/utility/tiny_gltf.h"

#include "ecs/components/MeshComponent.h"

namespace engine
{
	namespace loaders
	{
		class CGltfLoader
		{
		public:
			static void load(ecs::FMeshComponent& component);
		};
	}
}