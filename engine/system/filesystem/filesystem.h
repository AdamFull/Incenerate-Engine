#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace engine
{
	namespace system
	{
		struct fs
		{
			static std::string read_file(const std::filesystem::path& path);

			static void write_file(const std::filesystem::path& path, const std::string& data);
			static void write_file(const std::filesystem::path& path, std::vector<uint8_t>& binary);

			static bool is_file_exists(const std::filesystem::path& path);

			static std::filesystem::path get_workdir();
		};
	}
}