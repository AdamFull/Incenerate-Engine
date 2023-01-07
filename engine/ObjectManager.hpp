#pragma once

#include <atomic>
#include <queue>

namespace engine
{
	template<class _Ty>
	class CObjectManager
	{
	public:
		size_t add(const std::string& name, std::unique_ptr<_Ty>&& object)
		{
			mNameToId.emplace(name, next_id);
			mIdToObject.emplace(next_id, std::move(object));
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
				qFreedIds.push(id);
				mIdToObject.erase(objit);

				auto nameit = std::find_if(mNameToId.begin(), mNameToId.end(), [id](const auto& kv) { return kv.second == id; });
				if (nameit != mNameToId.end())
					mNameToId.erase(nameit);
			}
		}

		const std::unique_ptr<_Ty>& get(const std::string& name)
		{
			auto nameit = mNameToId.find(name);
			if (nameit != mNameToId.end())
				return get(nameit->second);
			return nullptr;
		}

		const std::unique_ptr<_Ty>& get(size_t id)
		{
			auto objit = mIdToObject.find(id);
			if (objit != mIdToObject.end())
				return objit->second;
			return nullptr;
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
		std::queue<size_t> qFreedIds;
		std::map<std::string, size_t> mNameToId;
		std::map<size_t, std::unique_ptr<_Ty>> mIdToObject;
	};
}