#pragma once
#include "GameObject.h"

namespace engine
{
	namespace game
	{
		class CMesh : public CGameObject
		{
		public:
			virtual ~CMesh() = default;

			void create() override;
			void reCreate() override;
			void render() override;
			void update(float fDeltaTime) override;
		};
	}
}