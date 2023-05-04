#pragma once

#include "EngineStructures.h"
#include "graphics/APIHandle.h"
#include "graphics/window/WindowAdapter.h"
#include "audio/AudioCore.h"
#include "physics/PhysicsCore.h"
#include "event/EventManager.hpp"
#include "scripting/ScriptingCore.h"
//#include "particles/ParticlesCore.h"

#include "game/SceneManager.h"
#include "ecs/systems/BaseSystem.h"
#include "ecs/components/fwd.h"

#include "filesystem/VirtualFileSystemManager.h"

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

	using winptr_t = std::unique_ptr<graphics::IWindowAdapter>;
	using graphptr_t = std::unique_ptr<graphics::CAPIHandle>;
	using audiocore_t = std::unique_ptr<audio::CAudioCore>;
	using scenemgr_t = std::unique_ptr<game::CSceneManager>;
	using scriptcore_t = std::unique_ptr<scripting::CScriptingCore>;
	using physicscore_t = std::unique_ptr<physics::CPhysicsCore>;
	//using particlescore_t = std::unique_ptr<particles::CParticlesCore>;
	using filesystem_t = std::unique_ptr<filesystem::CVirtualFileSystemManager>;

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

		const bool isEditorMode() const;
		const bool isEditorEditing() const;
		const bool isDebugDrawNormals() const;
		const bool isDebugDrawPhysics() const;

		void toggleDebugDrawNormals();
		void toggleDebugDrawPhysics();

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
		std::unique_ptr<CEventManager> pEventManager;

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

		bool bEditorMode{ true };
		bool bDebugDrawNormals{ false };
		bool bDebugDrawPhysics{ false };

		std::vector<std::unique_ptr<ecs::ISystem>> vSystems;
	};
}