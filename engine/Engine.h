#pragma once

#include <string>

#include <utility/upattern.hpp>

#include "EngineStructures.h"
#include "system/window/WindowHandle.h"
#include "system/input/InputMapper.h"
#include "graphics/APIHandle.h"
#include "game/Scene.h"

namespace engine
{
	using winptr_t = std::unique_ptr<system::window::CWindowHandle>;
	using inputptr_t = std::unique_ptr<system::input::CInputMapper>;
	using graphptr_t = std::unique_ptr<graphics::CAPIHandle>;
	using sceneptr_t = std::shared_ptr<game::CScene>;

	class CEngine : public utl::singleton<CEngine>
	{
	protected:
		CEngine();
	public:
		friend std::unique_ptr<CEngine> std::make_unique<CEngine>();

		~CEngine() = default;

		void create();

		void begin_render_loop();

		const winptr_t& getWindow() const;
		const inputptr_t& getInputMapper() const;
		const graphptr_t& getGraphics() const;

	private:
		winptr_t pWindow;
		inputptr_t pInputMapper;
		graphptr_t pGraphics;
		sceneptr_t pScene;
	};
}