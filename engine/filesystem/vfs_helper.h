#pragma once

namespace engine
{
	namespace filesystem
	{
		struct fs
		{
			static std::string get_mount_point(const std::string& path);
			static std::string to_posix_path(const std::string& path);
			static std::string get_local_path(const std::string& path);
		};
	}
}