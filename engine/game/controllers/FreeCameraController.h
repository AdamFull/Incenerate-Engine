#pragma once

#include "system/input/KeycodeHelper.hpp"
#include "CameraController.h"

namespace engine
{
	namespace game
	{
		class CFreeCameraController : public CCameraController
		{
		public:
			virtual ~CFreeCameraController() override = default;

			void create(system::input::CInputMapper* pInputMapper) override;
			void update(float fDT) override;

		private:
			void cameraMovement(system::input::EKeyCode eKey, system::input::EKeyState eState);
			void mouseRotation(float fX, float fY);
			void cameraToPoint(float fX, float fY);

		private:
			bool m_bRotatePass{ false };
		};
	}
}