#pragma once

#include "Type.hpp"
#include <array>
#include <queue>

namespace engine
{
	namespace ecs
	{
		class CEntityManager
		{
		public:
			CEntityManager();

			Entity create();
			void destroy(Entity entity);

			void setSignature(Entity entity, Signature signature);
			Signature getSignature(Entity entity);
		private:
			std::queue<Entity> mAvailableEntities{};
			std::array<Signature, MAX_ENTITIES> mSignatures{};
			uint32_t mLivingEntityCount{};
		};
	}
}