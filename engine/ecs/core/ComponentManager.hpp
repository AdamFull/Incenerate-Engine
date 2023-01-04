#pragma once

#include "ComponentArray.hpp"
#include "Type.hpp"
#include <any>
#include <memory>

namespace engine
{
	namespace ecs
	{
		class CComponentManager
		{
		public:
			template<class _Ty>
			void registrate()
			{
				const char* typeName = typeid(_Ty).name();
				assert(mComponentTypes.find(typeName) == mComponentTypes.end() && "Registering component type more than once.");

				mComponentTypes.insert({ typeName, mNextComponentType });
				mComponentArrays.insert({ typeName, std::make_unique<CComponentArray<_Ty>>() });

				++mNextComponentType;
			}

			template<class _Ty>
			ComponentType getType()
			{
				const char* typeName = typeid(_Ty).name();
				assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");
				return mComponentTypes[typeName];
			}

			template<class _Ty>
			void addComponent(Entity entity, _Ty component)
			{
				getArray<_Ty>()->insert(entity, component);
			}

			template<class _Ty>
			void removeComponent(Entity entity)
			{
				getArray<_Ty>()->remove(entity);
			}

			template<class _Ty>
			_Ty& getComponent(Entity entity)
			{
				return getArray<_Ty>()->get(entity);
			}

			void entityDestroyed(Entity entity)
			{
				for (auto const& [type, component] : mComponentArrays)
					component->entityDestroyed(entity);
			}
		private:
			std::unordered_map<const char*, ComponentType> mComponentTypes{};
			std::unordered_map<const char*, std::shared_ptr<IComponentArray>> mComponentArrays{};
			ComponentType mNextComponentType{};

			template<typename _Ty>
			CComponentArray<_Ty>* getArray()
			{
				const char* typeName = typeid(_Ty).name();
				assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");
				return static_cast<CComponentArray<_Ty>>(mComponentArrays[typeName].get());
			}
		};
	}
}