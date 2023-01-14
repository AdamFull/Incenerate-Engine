#pragma once

#include "event/Event.hpp"
#include "WindowBase.h"
#include "graphics/descriptors/DescriptorSet.h"

namespace engine
{
	namespace editor
	{
		class CEditorImageViewer : public IEditorWindow
		{
		public:
			CEditorImageViewer(const std::string& sname) { name = sname; }
			virtual ~CEditorImageViewer() override = default;

			void create() override;
			void __draw(float fDt) override;
		private:
			void OnOpenImage(CEvent& event);
		private:
			size_t openned_image{ invalid_index };
			std::unique_ptr<graphics::CDescriptorSet> pDescriptorSet;
		};
	}
}