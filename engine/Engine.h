#pragma once

#include <utility/upattern.hpp>

#include "EngineStructures.h"
#include "system/window/WindowHandle.h"
#include "graphics/APIHandle.h"
#include "audio/AudioCore.h"
#include "event/EventManager.hpp"
#include "editor/Editor.h"

#include "game/SceneGraph.hpp"
#include "ecs/systems/BaseSystem.h"

#define EGEngine CEngine::getInstance()
#define EGEditor CEngine::getInstance()->getEditor()
#define EGCoordinator CEngine::getInstance()->getCoordinator()
#define EGAudio CEngine::getInstance()->getAudio()
#define EGWindow CEngine::getInstance()->getWindow()
#define EGGraphics CEngine::getInstance()->getGraphics()
#define EGSceneGraph CEngine::getInstance()->getSceneGraph()

namespace engine
{
	enum class EEngineState
	{
		eEditing,
		ePause,
		ePlay
	};

	using editorptr_t = std::unique_ptr<editor::CEditor>;
	using winptr_t = std::unique_ptr<system::window::CWindowHandle>;
	using graphptr_t = std::unique_ptr<graphics::CAPIHandle>;
	using audiocore_t = std::unique_ptr<audio::CAudioCore>;

	class CEngine : public utl::singleton<CEngine>
	{
	protected:
		CEngine();
	public:
		friend std::unique_ptr<CEngine> std::make_unique<CEngine>();

		~CEngine();

		void create();

		void beginEngineLoop();

		entt::registry& getCoordinator();
		const editorptr_t& getEditor() const;
		const winptr_t& getWindow() const;
		const graphptr_t& getGraphics() const;
		const audiocore_t& getAudio() const;
		const entt::entity& getSceneGraph() const;

		const bool isEditorMode() const;

		void setState(EEngineState state) { eState = state; }
		EEngineState getState() { return eState; }

		// Event methods
		template<class... _Args>
		void addEventListener(EventId eventId, _Args&& ...args)
		{
			pEventManager->addListener(eventId, utl::function<void(CEvent&)>(std::forward<_Args>(args)...));
		}

		void sendEvent(CEvent& event);
		void sendEvent(EventId eventId);

	private:
		void destruction();
		void initEntityComponentSystem();
	private:
		entt::entity root;
		entt::registry registry;
		std::unique_ptr<CEventManager> pEventManager;

		EEngineState eState;

		editorptr_t pEditor;
		winptr_t pWindow;
		graphptr_t pGraphics;
		audiocore_t pAudio;
		bool bEditorMode{ true };

		std::vector<std::unique_ptr<ecs::ISystem>> vSystems;
	};
}