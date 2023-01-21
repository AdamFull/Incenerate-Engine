#pragma once

#include <utility/upattern.hpp>

#include "EngineStructures.h"
#include "system/window/WindowHandle.h"
#include "graphics/APIHandle.h"
#include "audio/AudioCore.h"
#include "event/EventManager.hpp"
#include "scripting/ScriptingCore.h"
#include "editor/Editor.h"

#include "game/SceneManager.h"
#include "ecs/systems/BaseSystem.h"

#define EGEngine CEngine::getInstance()
#define EGEditor CEngine::getInstance()->getEditor()
#define EGCoordinator CEngine::getInstance()->getCoordinator()
#define EGAudio CEngine::getInstance()->getAudio()
#define EGWindow CEngine::getInstance()->getWindow()
#define EGGraphics CEngine::getInstance()->getGraphics()
#define EGSceneManager CEngine::getInstance()->getSceneManager()

namespace engine
{
	struct FPostProcess
	{
		// fxaa
		bool fxaa{ true };
		float fxaa_quality{ 0.98f };

		// Tonemapping
		bool tonemapping{ true };
		float tonemapping_gamma{ 2.2f };
		float tonemapping_exposure{ 4.5f };

		// Bloom
		bool bloom{ true };
		float bloom_threshold{ 1.f };
		float bloom_filter_radius{ 0.005f };
		float bloom_strength{ 0.04f };

		// Chromatic aberration
		bool aberration{ false };
		float aberration_distortion{ 2.2f };
		int aberration_iterations{ 12 };

		// Vignette
		bool vignette{ false };
		float vignette_inner{ 0.15f };
		float vignette_outer{ 1.35f };
		float vignette_opacity{ 0.85f };
	};

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
	using scenemgr_t = std::unique_ptr<game::CSceneManager>;
	using scriptcore_t = std::unique_ptr<scripting::CScriptingCore>;

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
		const scenemgr_t& getSceneManager() const;
		const scriptcore_t& getScripting() const;
		FPostProcess& getPostEffects();

		const bool isEditorMode() const;

		void setState(EEngineState state) { eState = state; }
		EEngineState getState() { return eState; }

		const std::vector<std::unique_ptr<ecs::ISystem>>& getSystems() const;

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
		entt::registry registry;
		std::unique_ptr<CEventManager> pEventManager;

		EEngineState eState;

		FPostProcess posteffects;
		editorptr_t pEditor;
		winptr_t pWindow;
		graphptr_t pGraphics;
		audiocore_t pAudio;
		scenemgr_t pSceneManager;
		scriptcore_t pScripting;
		bool bEditorMode{ true };

		std::vector<std::unique_ptr<ecs::ISystem>> vSystems;
	};
}