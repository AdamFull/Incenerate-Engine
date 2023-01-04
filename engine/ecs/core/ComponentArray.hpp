#pragma once

#include <array>
#include <unordered_map>
#include <cassert>
#include "Type.hpp"

namespace engine
{
	namespace ecs
	{
		class IComponentArray
		{
		public:
			virtual ~IComponentArray() = default;
			virtual void entityDestroyed(Entity entity) = 0;
		};

		template<class _Ty>
		class CComponentArray : public IComponentArray
		{
		public:
			void insert(Entity entity, _Ty component)
			{
				assert(mEntityToIndexMap.find(entity) == mEntityToIndexMap.end() && "Component added to same entity more than once.");

				// Put new entry at end
				mEntityToIndexMap[entity] = mSize;
				mIndexToEntityMap[mSize] = entity;
				mComponentArray[mSize] = component;
				++mSize;
			}

			void remove(Entity entity)
			{
				assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Removing non-existent component.");

				// Copy element at end into deleted element's place to maintain density
				size_t indexOfRemovedEntity = mEntityToIndexMap[entity];
				size_t indexOfLastElement = mSize - 1;
				mComponentArray[indexOfRemovedEntity] = mComponentArray[indexOfLastElement];

				// Update map to point to moved spot
				Entity entityOfLastElement = mIndexToEntityMap[indexOfLastElement];
				mEntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
				mIndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

				mEntityToIndexMap.erase(entity);
				mIndexToEntityMap.erase(indexOfLastElement);

				--mSize;
			}

			_Ty& get(Entity entity)
			{
				assert(mEntityToIndexMap.find(entity) != mEntityToIndexMap.end() && "Retrieving non-existent component.");
				return mComponentArray[mEntityToIndexMap[entity]];
			}

			void entityDestroyed(Entity entity) override
			{
				if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end())
					remove(entity);
			}
		private:
			std::array<_Ty, MAX_ENTITIES> mComponentArray{};
			std::unordered_map<Entity, size_t> mEntityToIndexMap{};
			std::unordered_map<size_t, Entity> mIndexToEntityMap{};
			size_t mSize{};
		};
	}
}