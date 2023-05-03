#pragma once

#include "ie_vfs_exports.h"

#include <upattern.hpp>

namespace engine
{
	namespace filesystem
	{
        namespace format
        {
            using namespace utl;
            // images
            constexpr inline uint32_t ktx = ".ktx"_utl_hash;
            constexpr inline uint32_t ktx2 = ".ktx2"_utl_hash;
            constexpr inline uint32_t dds = ".dds"_utl_hash;
            constexpr inline uint32_t png = ".png"_utl_hash;
            constexpr inline uint32_t jpg = ".jpg"_utl_hash;
            constexpr inline size_t image_ts = 5;

            // meshes
            constexpr inline uint32_t gltf = ".gltf"_utl_hash;
            constexpr inline uint32_t glb = ".glb"_utl_hash;
            constexpr inline uint32_t obj = ".obj"_utl_hash;
            constexpr inline uint32_t fbx = ".fbx"_utl_hash;
            constexpr inline size_t mesh_ts = 2;

            // scripts
            constexpr inline uint32_t js = ".js"_utl_hash;
            constexpr inline size_t script_ts = 1;

            // scenes
            constexpr inline uint32_t iescene = ".iescene"_utl_hash; // incenerate scene
            constexpr inline size_t scene_ts = 1;

            // audio
            constexpr inline uint32_t wav = ".wav"_utl_hash;
            constexpr inline uint32_t wave = ".wave"_utl_hash;
            constexpr inline uint32_t ogg = ".ogg"_utl_hash;
            constexpr inline size_t audio_ts = 3;

            // particles
            constexpr inline uint32_t efkefc = ".efkefc"_utl_hash;
        }

		struct INCENERATE_VFS_EXPORT fs
		{
			static std::string get_mount_point(const std::string& path);
			static std::string to_posix_path(const std::string& path);
			static std::string get_local_path(const std::string& path);

            static std::string get_ext(const std::string& path);

            static std::string get_filename(const std::string& path);
            static std::string parent_path(const std::string& path);
            static std::string path_append(const std::string& base_path, const std::string& relative_path);
            static std::string normalize(const std::string& path);

            static bool is_scene_format(const std::string& path);
            static bool is_image_format(const std::string& path);
            static bool is_ktx_format(const std::string& path);
            static bool is_mesh_format(const std::string& path);
            static bool is_gltf_format(const std::string& path);
            static bool is_obj_format(const std::string& path);
            static bool is_fbx_format(const std::string& path);
            static bool is_skybox_format(const std::string& path);
            static bool is_script_format(const std::string& path);
            static bool is_audio_format(const std::string& path);
            static bool is_particle_format(const std::string& path);

        private:
            static bool is_scene_format(uint32_t format);
            static bool is_image_format(uint32_t format);
            static bool is_ktx_format(uint32_t format);
            static bool is_mesh_format(uint32_t format);
            static bool is_gltf_format(uint32_t format);
            static bool is_obj_format(uint32_t format);
            static bool is_fbx_format(uint32_t format);
            static bool is_skybox_format(uint32_t format);
            static bool is_script_format(uint32_t format);
            static bool is_audio_format(uint32_t format);
            static bool is_particle_format(uint32_t format);
		};
	}
}