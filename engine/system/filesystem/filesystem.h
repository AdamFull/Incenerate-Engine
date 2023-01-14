#pragma once

#include <utility/json.hpp>
#include <utility/upattern.hpp>
#include <bitset>

namespace engine
{
	namespace system
	{
        namespace format
        {
            using namespace utl;
            // images
            constexpr uint32_t ktx = ".ktx"_utl_hash;
            constexpr uint32_t ktx2 = ".ktx2"_utl_hash;
            constexpr uint32_t dds = ".dds"_utl_hash;
            constexpr uint32_t png = ".png"_utl_hash;
            constexpr uint32_t jpg = ".jpg"_utl_hash;
            constexpr size_t image_ts = 5;

            // meshes
            constexpr uint32_t gltf = ".gltf"_utl_hash;
            constexpr uint32_t glb = ".glb"_utl_hash;
            constexpr size_t mesh_ts = 2;

            // scripts
            constexpr uint32_t lua = ".lua"_utl_hash;
            constexpr size_t script_ts = 1;

            // scenes
            constexpr uint32_t is = ".is"_utl_hash; // incenerate scene
            constexpr uint32_t isb = ".isb"_utl_hash; // incenerate scene binary
            constexpr size_t scene_ts = 2;

            // audio
            constexpr uint32_t wav = ".wav"_utl_hash;
            constexpr uint32_t wave = ".wave"_utl_hash;
            constexpr uint32_t ogg = ".ogg"_utl_hash;
            constexpr size_t audio_ts = 3;
        }

		struct fs
		{
			static bool read_file(const std::filesystem::path& path, std::string& data, bool local = false);

			static bool write_file(const std::filesystem::path& path, const std::string& data, bool local = false);
			static bool write_file(const std::filesystem::path& path, std::vector<uint8_t>& binary, bool local = false);

			static bool is_file_exists(const std::filesystem::path& path);

            static std::string get_ext(const std::string& path);
            static std::string get_ext(const std::filesystem::path& path);

            static std::string get_filename(const std::string& path);
            static std::string get_filename(const std::filesystem::path& path);

            static bool is_image_format(const std::string& path);
            static bool is_image_format(const std::filesystem::path& path);

            static bool is_ktx_format(const std::string& path);
            static bool is_ktx_format(const std::filesystem::path& path);

            static bool is_mesh_format(const std::string& path);
            static bool is_mesh_format(const std::filesystem::path& path);

            static bool is_skybox_format(const std::string& path);
            static bool is_skybox_format(const std::filesystem::path& path);

            static bool is_script_format(const std::string& path);
            static bool is_script_format(const std::filesystem::path& path);

            static bool is_audio_format(const std::string& path);
            static bool is_audio_format(const std::filesystem::path& path);

            template<class _Ty>
            static bool read_json(const std::filesystem::path& path, _Ty& type, bool local = false)
            {
                std::string data{};
                if (read_file(path, data, local))
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
            static bool write_json(const std::filesystem::path& path, const _Ty& type, int32_t indent = -1, bool local = false)
            {
                auto json = nlohmann::json(type);
                if (!json.empty())
                {
                    auto data = json.dump(indent);
                    if (!data.empty())
                        return write_file(path, data, local);
                }
                
                return false;
            }

            template<class _Ty>
            static bool read_bson(const std::filesystem::path& path, _Ty& type, bool local = false)
            {
                std::string data{};
                if (read_file(path, data, local))
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
            static bool write_bson(const std::filesystem::path& path, const _Ty& type, bool local = false)
            {
                auto json = nlohmann::json(type);
                if (!json.empty())
                {
                    auto data = nlohmann::json::to_bson(json);
                    if (!data.empty())
                        return write_file(path, data, local);
                }

                return false;
            }

            // It's should be project dir
			static std::filesystem::path get_workdir(bool local = false);

        private:
            static bool is_image_format(uint32_t format);
            static bool is_ktx_format(uint32_t format);
            static bool is_mesh_format(uint32_t format);
            static bool is_skybox_format(uint32_t format);
            static bool is_script_format(uint32_t format);
            static bool is_audio_format(uint32_t format);
		};
	}
}