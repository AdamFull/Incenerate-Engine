#pragma once

#include <string>

#include <utility/upattern.hpp>

#include "EngineStructures.h"
#include "ecs/core/Coordinator.hpp"
#include "system/window/WindowHandle.h"
#include "graphics/APIHandle.h"

#include "game/SceneGraph.hpp"

#define EGEngine CEngine::getInstance()
#define EGCoordinator CEngine::getInstance()->getCoordinator()
#define EGWindow CEngine::getInstance()->getWindow()
#define EGGraphics CEngine::getInstance()->getGraphics()

namespace engine
{
	using coordinator_t = std::unique_ptr<ecs::CCoordinator>;
	using winptr_t = std::unique_ptr<system::window::CWindowHandle>;
	using graphptr_t = std::unique_ptr<graphics::CAPIHandle>;

	class CEngine : public utl::singleton<CEngine>
	{
	protected:
		CEngine();
	public:
		friend std::unique_ptr<CEngine> std::make_unique<CEngine>();

		~CEngine();

		void create();

		void beginEngineLoop();

		const coordinator_t& getCoordinator() const;
		const winptr_t& getWindow() const;
		const graphptr_t& getGraphics() const;

	private:
		void initEntityComponentSystem();
	private:
		std::unique_ptr<game::CSceneNode> pRoot;
		coordinator_t pCoordinator;
		winptr_t pWindow;
		graphptr_t pGraphics;

		std::vector<ecs::ISystem*> vSystems;
	};
}