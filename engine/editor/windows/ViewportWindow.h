#pragma once

#include "WindowBase.h"
#include "graphics/descriptors/DescriptorSet.h"

namespace engine
{
	namespace editor
	{
		class CEditorViewport : public CEditorWindow
		{
		public:
			virtual ~CEditorViewport() override;

			void create() override;
			void draw() override;
		private:
			void drawViewport(float offsetx, float offsety);
			void drawManipulator(float offsetx, float offsety, float sizex, float sizey);
			void drawOverlay(float offsetx, float offsety);

		private:
			std::unique_ptr<graphics::CDescriptorSet> pDescriptorSet;
			float overlayX{ 300.0 }, overlayY{ 50.f };
		};
	}
}