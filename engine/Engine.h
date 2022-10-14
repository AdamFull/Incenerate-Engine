#pragma once

#include <string>

#include <utility/upattern.hpp>

#include "EngineStructures.h"
#include "system/window/WindowHandle.h"
#include "system/input/InputMapper.h"
#include "graphics/APIHandle.h"
#include "FreeCameraController.h"

namespace engine
{
	using winptr_t = std::unique_ptr<system::window::CWindowHandle>;
	using inputptr_t = std::unique_ptr<system::input::CInputMapper>;
	using graphptr_t = std::unique_ptr<graphics::CAPIHandle>;
	using camctrlptr_t = std::unique_ptr<CFreeCameraController>;

	class CEngine : public utl::singleton<CEngine>
	{
	public:
		friend std::unique_ptr<CEngine> std::make_unique<CEngine>();

		~CEngine() = default;

		void create(const FEngineCreateInfo& createInfo);

		void begin_render_loop();

		const winptr_t& getWindow() const;
		const inputptr_t& getInputMapper() const;
		const graphptr_t& getGraphics() const;
		const camctrlptr_t& getCameraController() const;

	private:
		winptr_t pWindow;
		inputptr_t pInputMapper;
		graphptr_t pGraphics;
		camctrlptr_t pCameraController;
	};
}