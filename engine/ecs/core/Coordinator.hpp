#pragma once

#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include "EventManager.hpp"
#include "SystemManager.hpp"

namespace engine
{
	namespace ecs
	{
		class CCoordinator
		{
		public:
			CCoordinator() = default;

			void create();

			// Entity methods
			Entity createEntity();
			void destroyEntity(Entity entity);

			// Component methods
			template<class _Ty>
			void registerComponent()
			{
				pComponentManager->registrate<_Ty>();
			}

			template<class _Ty>
			void addComponent(Entity entity, _Ty component)
			{
				pComponentManager->addComponent<_Ty>(entity, component);

				auto signature = pEntityManager->getSignature(entity);
				signature.set(pComponentManager->getType<_Ty>(), true);
				pEntityManager->setSignature(entity, signature);

				pSystemManager->entitySignatureChanged(entity, signature);
			}

			template<class _Ty>
			void removeComponent(Entity entity)
			{
				pComponentManager->removeComponent<_Ty>(entity);

				auto signature = pEntityManager->getSignature(entity);
				signature.set(pComponentManager->getType<_Ty>(), false);
				pEntityManager->setSignature(entity, signature);

				pSystemManager->entitySignatureChanged(entity, signature);
			}

			template<class _Ty>
			_Ty& getComponent(Entity entity)
			{
				return pComponentManager->getComponent<_Ty>(entity);
			}

			template<class _Ty>
			ComponentType getComponentType()
			{
				return pComponentManager->getType<_Ty>();
			}

			// System methods
			template<class _Ty>
			_Ty* registerSystem()
			{
				return pSystemManager->registrate<_Ty>();
			}

			template<class _Ty>
			void setSystemSignature(Signature signature)
			{
				pSystemManager->setSignature<_Ty>(signature);
			}

			// Event methods
			template<class... _Args>
			void addEventListener(EventId eventId, _Args&& ...args)
			{
				pEventManager->addListener(eventId, utl::function<void(CEvent&)>(std::forward<_Args>(args)...));
			}

			void sendEvent(CEvent& event);
			void sendEvent(EventId eventId);
		private:
			std::unique_ptr<CComponentManager> pComponentManager;
			std::unique_ptr<CEntityManager> pEntityManager;
			std::unique_ptr<CEventManager> pEventManager;
			std::unique_ptr<CSystemManager> pSystemManager;
		};
	}
}