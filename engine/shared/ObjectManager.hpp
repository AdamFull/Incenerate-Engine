#pragma once

#include <atomic>
#include <queue>
#include <unordered_map>
#include <memory>
#include <string>

#include "SharedInstances.h"

namespace engine
{
	template<class _Ty>
	class CObjectManager
	{
	public:
		~CObjectManager()
		{
			performDeletion();
		}

		size_t add(const std::string& name, std::unique_ptr<_Ty>&& object)
		{
			auto id = getId(name);
			if (id != invalid_index)
			{
				mUsages[id].fetch_add(1, std::memory_order_relaxed);
				return id;
			}

			mNameToId.emplace(name, next_id);
			mIdToObject.emplace(next_id, std::move(object));
			mUsages[next_id].fetch_add(1, std::memory_order_relaxed);
			nextId();
			return mNameToId[name];
		}

		void increment(const std::string& name)
		{
			increment(getId(name));
		}

		void increment(size_t id)
		{
			if (id != invalid_index)
				mUsages[id].fetch_add(1, std::memory_order_relaxed);
		}

		void remove(const std::string& name)
		{
			auto nameit = mNameToId.find(name);
			if (nameit != mNameToId.end())
				remove(nameit->second);
		}

		void remove(size_t id)
		{
			auto objit = mIdToObject.find(id);
			if (objit != mIdToObject.end())
			{
				auto usages = mUsages.find(id);
				usages->second.fetch_sub(1, std::memory_order_relaxed);
				if (usages->second == 0ull)
				{
					qFreedIds.push(id);
					qDelete.push(std::move(objit->second));
					mIdToObject.erase(objit);
					mUsages.erase(usages);

					auto nameit = std::find_if(mNameToId.begin(), mNameToId.end(), [id](const auto& kv) { return kv.second == id; });
					if (nameit != mNameToId.end())
						mNameToId.erase(nameit);
				}
			}
		}

		size_t getId(const std::string& name)
		{
			auto nameit = mNameToId.find(name);
			if (nameit != mNameToId.end())
				return nameit->second;
			return invalid_index;
		}

		const std::unique_ptr<_Ty>& get(const std::string& name)
		{
			static std::unique_ptr<_Ty> pEmpty{ nullptr };
			auto id = getId(name);
			if (id != invalid_index)
				return get(id);
			return pEmpty;
		}

		const std::unique_ptr<_Ty>& get(size_t id)
		{
			static std::unique_ptr<_Ty> pEmpty{ nullptr };
			auto objit = mIdToObject.find(id);
			if (objit != mIdToObject.end())
				return objit->second;
			return pEmpty;
		}

		void performDeletion()
		{
			while (!qDelete.empty())
				qDelete.pop();
		}

		const std::unordered_map<size_t, std::unique_ptr<_Ty>>& getAll() const { return mIdToObject; }

	private:
		void nextId()
		{
			static std::atomic<size_t> uid{ 0 };

			if (!qFreedIds.empty())
			{
				next_id = qFreedIds.front();
				qFreedIds.pop();
			}
			else
			{
				uid.fetch_add(1, std::memory_order_relaxed);

				if (uid == invalid_index)
					uid.fetch_add(1, std::memory_order_relaxed);

				next_id = uid;
			}
		}

	private:
		size_t next_id{ 0 };
		std::queue<std::unique_ptr<_Ty>> qDelete;
		std::queue<size_t> qFreedIds;
		std::unordered_map<size_t, std::atomic<size_t>> mUsages;
		std::unordered_map<std::string, size_t> mNameToId;
		std::unordered_map<size_t, std::unique_ptr<_Ty>> mIdToObject;
	};
}