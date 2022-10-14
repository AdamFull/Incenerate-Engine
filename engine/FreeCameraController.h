#pragma once

#include "system/input/KeycodeHelper.hpp"
#include "Camera.h"

namespace engine
{
	namespace system
	{
		namespace input { class CInputMapper; }
	}

	class CFreeCameraController
	{
	public:
		void create(system::input::CInputMapper* pInputMapper);
		void update(float fDT);

		const std::unique_ptr<CCamera>& getCamera();

	private:
		void cameraMovement(system::input::EKeyCode eKey, system::input::EKeyState eState);
		void mouseRotation(float fX, float fY);
		void cameraToPoint(float fX, float fY);

	private:
		std::unique_ptr<CCamera> pCamera;
		bool m_bRotatePass{ false };
	};
}