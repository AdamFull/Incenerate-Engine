#pragma once

#include <atomic>
#include <queue>

namespace engine
{
	template<class _Ty>
	class CObjectManager
	{
	public:
		~CObjectManager()
		{
			perform_deletion();
		}

		size_t add(const std::string& name, std::unique_ptr<_Ty>&& object)
		{
			auto id = get_id(name);
			if (id != invalid_index)
			{
				mUsages[id].fetch_add(1, std::memory_order_relaxed);
				return id;
			}

			mNameToId.emplace(name, next_id);
			mIdToObject.emplace(next_id, std::move(object));
			mUsages[next_id].fetch_add(1, std::memory_order_relaxed);
			nextid();
			return mNameToId[name];
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

		size_t get_id(const std::string& name)
		{
			auto nameit = mNameToId.find(name);
			if (nameit != mNameToId.end())
				return nameit->second;
			return invalid_index;
		}

		const std::unique_ptr<_Ty>& get(const std::string& name)
		{
			auto id = get_id(name);
			if (id != invalid_index)
				return get(id);
			return nullptr;
		}

		const std::unique_ptr<_Ty>& get(size_t id)
		{
			auto objit = mIdToObject.find(id);
			if (objit != mIdToObject.end())
				return objit->second;
			return nullptr;
		}

		void perform_deletion()
		{
			while (!qDelete.empty())
				qDelete.pop();
		}

	private:
		void nextid()
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
		std::map<size_t, std::atomic<size_t>> mUsages;
		std::map<std::string, size_t> mNameToId;
		std::map<size_t, std::unique_ptr<_Ty>> mIdToObject;
	};
}