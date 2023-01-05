#pragma once

#include "ecs/components/MeshComponent.h"

namespace engine
{
	namespace loaders
	{
		class CMeshLoader
		{
		public:
			static void load(ecs::FMeshComponent& component);
		};
	}
}