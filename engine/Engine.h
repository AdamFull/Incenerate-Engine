#pragma once

#include "EngineStructures.h"
#include "graphics/APIHandle.h"
#include "graphics/window/WindowAdapter.h"
#include "physics/PhysicsCore.h"
//#include "particles/ParticlesCore.h"

#include "game/SceneManager.h"
#include "ecs/systems/BaseSystem.h"
#include "ecs/components/fwd.h"

#include <interface/filesystem/VirtualFileSystemInterface.h>
#include <interface/audio/AudioCoreInterface.h>
#include <interface/event/EventManagerInterface.h>
#include <interface/scripting/ScriptingCoreInterface.h>

#include <utility/threading.hpp>

#define EGEngine CEngine::getInstance()
#define EGScripting CEngine::getInstance()->getScripting()
#define EGFilesystem CEngine::getInstance()->getFilesystem()
#define EGAudio CEngine::getInstance()->getAudio()
#define EGWindow CEngine::getInstance()->getWindow()
#define EGSceneManager CEngine::getInstance()->getSceneManager()

namespace engine
{
	enum class EEngineState
	{
		eEditing,
		ePause,
		ePlay
	};

	using audiocore_t = std::unique_ptr<audio::IAudioSystemInterface>;
	using eventcore_t = std::unique_ptr<IEventManagerInterface>;
	using winptr_t = std::unique_ptr<graphics::IWindowAdapter>;
	using graphptr_t = std::unique_ptr<graphics::CAPIHandle>;
	
	using scenemgr_t = std::unique_ptr<game::CSceneManager>;
	using scriptcore_t = std::unique_ptr<scripting::IScriptingCoreInterface>;
	using physicscore_t = std::unique_ptr<physics::CPhysicsCore>;
	//using particlescore_t = std::unique_ptr<particles::CParticlesCore>;
	using filesystem_t = std::unique_ptr<filesystem::IVirtualFileSystemInterface>;

	class CEngine : public utl::singleton<CEngine>
	{
	protected:
		CEngine();
	public:
		friend std::unique_ptr<CEngine> std::make_unique<CEngine>();

		~CEngine();

		void initialize();

		void create();

		void beginEngineLoop();

		entt::registry& getRegistry();

		const winptr_t& getWindow() const;
		const graphptr_t& getGraphics() const;
		const audiocore_t& getAudio() const;
		const scenemgr_t& getSceneManager() const;
		const scriptcore_t& getScripting() const;
		const physicscore_t& getPhysics() const;
		//const particlescore_t& getParticles() const;
		const filesystem_t& getFilesystem() const;

		const std::unique_ptr<utl::threadworker>& getLoaderThread();

		const bool isEditorEditing() const;

		void setState(EEngineState state) { eState = state; }
		EEngineState getState() { return eState; }

		const std::vector<std::unique_ptr<ecs::ISystem>>& getSystems() const;

		std::unique_ptr<IEvent> makeEvent(EventId eventId);

		// Event methods
		template<class... _Args>
		void addEventListener(EventId eventId, _Args&& ...args)
		{
			pEventManager->addListener(eventId, utl::function<void(const std::unique_ptr<IEvent>&)>(std::forward<_Args>(args)...));
		}

		void sendEvent(const std::unique_ptr<IEvent>& event);
		void sendEvent(EventId eventId);

		ecs::FCameraComponent* getActiveCamera();

		template<class _Ty>
		void addSystem()
		{
			vSystems.emplace_back(std::make_unique<_Ty>());
		}

	private:
		void destruction();
		void initEntityComponentSystem();
	private:
		entt::registry registry;
		eventcore_t pEventManager;

		EEngineState eState;

		winptr_t pWindow;
		graphptr_t pGraphics;
		audiocore_t pAudio;
		scenemgr_t pSceneManager;
		scriptcore_t pScripting;
		physicscore_t pPhysics;
		//particlescore_t pParticles;
		filesystem_t pFilesystem;

		std::unique_ptr<utl::threadworker> pLoaderThread;

		bool bEditorMode{ false };

		std::vector<std::unique_ptr<ecs::ISystem>> vSystems;
	};
}