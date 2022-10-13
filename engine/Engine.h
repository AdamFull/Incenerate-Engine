#pragma once

#include <string>

#include <utility/upattern.hpp>

#include "EngineStructures.h"
#include "system/window/WindowHandle.h"
#include "graphics/APIHandle.h"

namespace engine
{
	using winptr_t = std::unique_ptr<system::window::CWindowHandle>;
	using graphptr_t = std::unique_ptr<graphics::CAPIHandle>;

	class CEngine : public utl::singleton<CEngine>
	{
	public:
		friend std::unique_ptr<CEngine> std::make_unique<CEngine>();

		~CEngine() = default;

		void create(const FEngineCreateInfo& createInfo);

		const winptr_t& getWindow() const;
		const graphptr_t& getGraphics() const;

	private:
		winptr_t pWindow;
		graphptr_t pGraphics;
	};
}