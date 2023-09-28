#pragma once

#include <any>
#include <unordered_map>

#include <upattern.hpp>

namespace engine
{
	class CSessionStorage : public utl::singleton<CSessionStorage>
	{
	public:
		template<typename T>
		void set(const std::string& key, T value) 
		{
			storage[key] = value;
		}

		template<typename T>
		T get(const std::string& key, T defaultValue = T()) const 
		{
			auto it = storage.find(key);
			if (it == storage.end())
				return defaultValue;

			const std::any& value = it->second;
			if (typeid(T) != value.type())
			{
				log_error("Type mismatch for key: {}", key);
				throw std::runtime_error("Type mismatch for key: " + key);
			}

			return std::any_cast<T>(value);
		}

		bool has(const std::string& key) const 
		{
			return storage.count(key) > 0;
		}

		void erase(const std::string& key) 
		{
			storage.erase(key);
		}

		void clear() 
		{
			storage.clear();
		}
	private:
		std::unordered_map<std::string, std::any> storage;
	};
}