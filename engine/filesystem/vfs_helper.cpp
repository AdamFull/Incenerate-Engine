#include "vfs_helper.h"

using namespace engine::filesystem;

std::string fs::get_mount_point(const std::string& path)
{
    auto first_separator = std::find(path.begin(), path.end(), '/');

    std::string result = '/' + std::string(path.begin(), first_separator);

    if (first_separator != path.end())
    {
        ++first_separator;
        auto second_separator = std::find(first_separator, path.end(), '/');
        result += std::string(first_separator, second_separator);
    }

    return result;
}

std::string fs::to_posix_path(const std::string& path)
{
	auto result = path;
	std::replace(result.begin(), result.end(), '\\', '/');
	return result;
}

std::string fs::get_local_path(const std::string& path)
{
    std::string mount_point = get_mount_point(path);
    std::string posix_path = to_posix_path(path);

    if (posix_path.find(mount_point) != 0)
        return "";

    std::string local_path = posix_path.substr(mount_point.length());

    if (!local_path.empty() && local_path.front() == '/')
        local_path.erase(local_path.begin());

    return local_path;
}