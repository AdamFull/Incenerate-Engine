#pragma once

#include <utility/json.hpp>
#include <filesystem>
#include <string>
#include <vector>

namespace engine
{
	namespace system
	{
		struct fs
		{
			static bool read_file(const std::filesystem::path& path, std::string& data);

			static bool write_file(const std::filesystem::path& path, const std::string& data);
			static bool write_file(const std::filesystem::path& path, std::vector<uint8_t>& binary);

			static bool is_file_exists(const std::filesystem::path& path);

            template<class _Ty>
            static bool read_json(const std::filesystem::path& path, _Ty& type)
            {
                std::string data{};
                if (read_file(path, data))
                {
                    if (!data.empty())
                    {
                        auto json = nlohmann::json::parse(data);
                        if (!json.empty())
                        {
                            json.get_to(type);
                            return true;
                        }
                    }
                }
                
                return false;
            }

            template<class _Ty>
            static bool write_json(const std::filesystem::path& path, const _Ty& type, int32_t indent = -1)
            {
                auto json = nlohmann::json(type);
                if (!json.empty())
                {
                    auto data = json.dump(indent);
                    if (!data.empty())
                        return write_file(path, data);
                }
                
                return false;
            }

            template<class _Ty>
            static bool read_bson(const std::filesystem::path& path, _Ty& type)
            {
                std::string data{};
                if (read_file(path, data))
                {
                    auto json = nlohmann::json::from_bson(data);
                    if (!json.empty())
                    {
                        json.get_to(type);
                        return true;
                    }
                }

                return false;
            }

            template<class _Ty>
            static bool write_bson(const std::filesystem::path& path, const _Ty& type)
            {
                auto json = nlohmann::json(type);
                if (!json.empty())
                {
                    auto data = nlohmann::json::to_bson(json);
                    if (!data.empty())
                        return write_file(path, data);
                }

                return false;
            }

			static std::filesystem::path get_workdir();
		};
	}
}