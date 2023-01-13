#include "filesystem.h"

#include <fstream>

using namespace engine::system;

bool fs::read_file(const std::filesystem::path& path, std::string& data, bool local)
{
    auto full_path = get_workdir(local) / path;

    if (is_file_exists(full_path))
    {
        std::ifstream file(full_path, std::ios_base::in | std::ios_base::binary);
        file.rdbuf()->pubsetbuf(0, 0);

        data = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        return true;
    }
    

    return false;
}

bool fs::write_file(const std::filesystem::path& path, const std::string& data, bool local)
{
    auto full_path = get_workdir(local) / path;
    std::ofstream file(full_path, std::ios::out | std::ios::binary);
    file << data;
    file.close();

    return true;
}

bool fs::write_file(const std::filesystem::path& path, std::vector<uint8_t>& binary, bool local)
{
    auto full_path = get_workdir(local) / path;
    std::ofstream file(full_path, std::ios::out | std::ios::binary);
    file.write((char*)&binary[0], binary.size() * sizeof(binary[0]));
    file.close();

    return true;
}

bool fs::is_file_exists(const std::filesystem::path& path)
{
    return std::filesystem::exists(path);
}

std::string fs::get_ext(const std::string& path)
{
    if (auto fnd = path.find_last_of("."))
    {
        if (fnd != std::string::npos)
            return path.substr(fnd);
    }

    return "";
}

std::string fs::get_ext(const std::filesystem::path& path)
{
    return path.extension().string();
}

std::string fs::get_filename(const std::string& path)
{
    return path.substr(path.find_last_of("/\\") + 1);
}

std::string fs::get_filename(const std::filesystem::path& path)
{
    return path.filename().string();
}

bool fs::is_image_format(const std::string& path)
{
    auto ext = get_ext(path);
    return is_image_format(utl::const_hash(ext.c_str(), ext.size()));
}

bool fs::is_image_format(const std::filesystem::path& path)
{
    auto ext = get_ext(path);
    return is_image_format(utl::const_hash(ext.c_str(), ext.size()));
}

bool fs::is_mesh_format(const std::string& path)
{
    auto ext = get_ext(path);
    return is_mesh_format(utl::const_hash(ext.c_str(), ext.size()));
}

bool fs::is_mesh_format(const std::filesystem::path& path)
{
    auto ext = get_ext(path);
    return is_mesh_format(utl::const_hash(ext.c_str(), ext.size()));
}

bool fs::is_skybox_format(const std::string& path)
{
    auto ext = get_ext(path);
    return is_skybox_format(utl::const_hash(ext.c_str(), ext.size()));
}

bool fs::is_skybox_format(const std::filesystem::path& path)
{
    auto ext = get_ext(path);
    return is_skybox_format(utl::const_hash(ext.c_str(), ext.size()));
}

bool fs::is_script_format(const std::string& path)
{
    auto ext = get_ext(path);
    return is_script_format(utl::const_hash(ext.c_str(), ext.size()));
}

bool fs::is_script_format(const std::filesystem::path& path)
{
    auto ext = get_ext(path);
    return is_script_format(utl::const_hash(ext.c_str(), ext.size()));
}

bool fs::is_audio_format(const std::string& path)
{
    auto ext = get_ext(path);
    return is_audio_format(utl::const_hash(ext.c_str(), ext.size()));
}

bool fs::is_audio_format(const std::filesystem::path& path)
{
    auto ext = get_ext(path);
    return is_audio_format(utl::const_hash(ext.c_str(), ext.size()));
}

std::filesystem::path fs::get_workdir(bool local)
{
    if (local)
        return std::filesystem::current_path();
    else
        return std::filesystem::current_path().assign(std::filesystem::weakly_canonical("../assets/"));
}

// TODO: refactor this
bool fs::is_image_format(uint32_t format)
{
    return 
        format == format::ktx || 
        format == format::ktx2 || 
        format == format::dds || 
        format == format::png || 
        format == format::jpg;
}

bool fs::is_mesh_format(uint32_t format)
{
    return 
        format == format::gltf ||
        format == format::glb;
}

bool fs::is_skybox_format(uint32_t format)
{
    return 
        format == format::ktx ||
        format == format::ktx2 ||
        format == format::dds;
}

bool fs::is_script_format(uint32_t format)
{
    return format == format::lua;
}

bool fs::is_audio_format(uint32_t format)
{
    return
        format == format::wav ||
        format == format::wave ||
        format == format::ogg;
}
