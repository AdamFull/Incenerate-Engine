#include "filesystem.h"

#include <fstream>

using namespace engine::system;

bool fs::read_file(const std::filesystem::path& path, std::string& data)
{
    auto full_path = get_workdir() / path;

    if (is_file_exists(full_path))
    {
        std::ifstream file(full_path, std::ios_base::in | std::ios_base::binary);
        file.rdbuf()->pubsetbuf(0, 0);

        data = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
        return true;
    }
    

    return false;
}

bool fs::write_file(const std::filesystem::path& path, const std::string& data)
{
    auto full_path = get_workdir() / path;
    std::ofstream file(full_path, std::ios::out | std::ios::binary);
    file << data;
    file.close();

    return true;
}

bool fs::write_file(const std::filesystem::path& path, std::vector<uint8_t>& binary)
{
    auto full_path = get_workdir() / path;
    std::ofstream file(full_path, std::ios::out | std::ios::binary);
    file.write((char*)&binary[0], binary.size() * sizeof(binary[0]));
    file.close();

    return true;
}

bool fs::is_file_exists(const std::filesystem::path& path)
{
    return std::filesystem::exists(path);
}

std::filesystem::path fs::get_workdir()
{
    return std::filesystem::current_path().assign(std::filesystem::weakly_canonical("../../assets/"));
}