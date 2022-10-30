#pragma once

#include "game/Camera.h"

namespace engine
{
	namespace system { namespace input { class CInputMapper; } }

	namespace game
	{
		class CCameraController
		{
		public:
			virtual ~CCameraController() = default;

			virtual void create(system::input::CInputMapper* pInputMapper);
			virtual void update(float fDT);

			void setCamera(std::shared_ptr<CCamera> camera);
			const std::shared_ptr<CCamera> getCamera();

		protected:
			std::shared_ptr<CCamera> pCamera;
		};
	}
}