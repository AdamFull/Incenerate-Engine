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
			//void onViewportReCreate(CEvent& event);
			void drawViewport(float offsetx, float offsety);
			void drawManipulator(float offsetx, float offsety, float sizex, float sizey);
			void drawOverlay(float offsetx, float offsety, float fDt);

		private:
			std::unique_ptr<graphics::CDescriptorSet> pDescriptorSet;
			float overlayX{ 300.0 }, overlayY{ 50.f };
			bool isManipulating{ false };
		};
	}
}