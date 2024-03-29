#pragma once

#include "WindowBase.h"
#include "graphics/descriptors/DescriptorSet.h"

namespace engine
{
	namespace editor
	{
		class CEditorViewport : public IEditorWindow
		{
		public:
			CEditorViewport(const std::string& sname) { name = sname; }
			virtual ~CEditorViewport() override;

			void create() override;
			void __draw(float fDt) override;
		private:
			//void onViewportReCreate(const std::unique_ptr<IEvent>& event);
			void drawViewport();
			void viewportPicking();
			void drawManipulator(float offsetx, float offsety, float sizex, float sizey);
			void drawOverlay(float offsetx, float offsety, float fDt);

		private:
			graphics::CAPIHandle* graphics{ nullptr };
			std::unique_ptr<graphics::CDescriptorSet> pDescriptorSet;
			float overlayX{ 300.0 }, overlayY{ 50.f };
			float viewportPanelSizeX{ 0.f }, viewportPanelSizeY{ 0.f };
			bool isManipulating{ false };
			uint16_t frameRate{ 0 };
			uint16_t frameRateCounter{ 0 };
			float frameRateAccum{ 0.f };
		};
	}
}