#include "filesystem.h"

#include <fstream>

using namespace engine::system;

std::string fs::read_file(const std::filesystem::path& path)
{
    auto full_path = get_workdir() / path;
    std::ifstream file(full_path, std::ios_base::in | std::ios_base::binary);
    file.rdbuf()->pubsetbuf(0, 0);
    file.imbue(std::locale(std::locale::empty(), new std::codecvt<char16_t, char, std::mbstate_t>));

    return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

void fs::write_file(const std::filesystem::path& path, const std::string& data)
{
    auto full_path = get_workdir() / path;
    std::ofstream file(full_path, std::ios::out | std::ios::binary);
    file << data;
    file.close();
}

void fs::write_file(const std::filesystem::path& path, std::vector<uint8_t>& binary)
{
    auto full_path = get_workdir() / path;
    std::ofstream file(full_path, std::ios::out | std::ios::binary);
    file.write((char*)&binary[0], binary.size() * sizeof(binary[0]));
    file.close();
}

bool fs::is_file_exists(const std::filesystem::path& path)
{
    return std::filesystem::exists(path);
}

std::filesystem::path fs::get_workdir()
{
    return std::filesystem::current_path().assign(std::filesystem::weakly_canonical("../../assets/"));
}