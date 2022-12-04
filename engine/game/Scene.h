#pragma once

#include "controllers/CameraController.h"

namespace engine
{
	namespace game
	{
		class CScene : public CGameObject
		{
		public:
			CScene();
			virtual ~CScene() override = default;

			void create() override;
			void render() override;
			void update(float fDT) override;

		protected:
			std::unique_ptr<CCameraController> pCameraController;
		};
	}
}