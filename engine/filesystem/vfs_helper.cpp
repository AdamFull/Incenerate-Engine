#include "vfs_helper.h"

#include <algorithm>
#include <deque>
#include <sstream>
#include <codecvt>

using namespace engine::filesystem;

std::string fs::from_unicode(const std::u16string& data)
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return convert.to_bytes(data);
}

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

std::string fs::get_ext(const std::string& path)
{
	if (auto fnd = path.find_last_of("."))
	{
		if (fnd != std::string::npos)
			return path.substr(fnd);
	}

	return "";
}

std::string fs::get_filename(const std::string& path)
{
	return path.substr(path.find_last_of("/\\") + 1);
}

std::string fs::parent_path(const std::string& path)
{
	const size_t separator_pos = path.find_last_of("/\\");
	return (separator_pos == std::string::npos) ? path : path.substr(0, separator_pos);
}

std::string fs::path_append(const std::string& base_path, const std::string& relative_path)
{
	if (base_path.empty())
		return relative_path;

	if (relative_path.empty())
		return base_path;

	std::string result_path = base_path;
	if (base_path.back() != '/')
	{
		if(relative_path.front() != '/')
			result_path += '/';
	}

	result_path += relative_path;
	return result_path;
}

std::string fs::normalize(const std::string& path)
{
	std::deque<std::string> components;
	std::istringstream iss(path);
	std::string part;

	while (std::getline(iss, part, '/')) 
	{
		if (part.empty() || part == ".")
			continue;

		else if (part == "..")
		{
			if (!components.empty())
				components.pop_back();
		}
		else
			components.push_back(std::move(part));
	}

	std::ostringstream oss;
	for (const auto& component : components)
		oss << '/' << component;

	return oss.str().empty() ? "/" : oss.str();
}

bool fs::is_scene_format(const std::string& path)
{
	auto ext = get_ext(path);
	return is_scene_format(utl::const_hash(ext.c_str(), ext.size()));
}

bool fs::is_image_format(const std::string& path)
{
	auto ext = get_ext(path);
	return is_image_format(utl::const_hash(ext.c_str(), ext.size()));
}

bool fs::is_ktx_format(const std::string& path)
{
	auto ext = get_ext(path);
	return is_ktx_format(utl::const_hash(ext.c_str(), ext.size()));
}

bool fs::is_mesh_format(const std::string& path)
{
	auto ext = get_ext(path);
	return is_mesh_format(utl::const_hash(ext.c_str(), ext.size()));
}

bool fs::is_gltf_format(const std::string& path)
{
	auto ext = get_ext(path);
	return is_gltf_format(utl::const_hash(ext.c_str(), ext.size()));
}

bool fs::is_obj_format(const std::string& path)
{
	auto ext = get_ext(path);
	return is_obj_format(utl::const_hash(ext.c_str(), ext.size()));
}

bool fs::is_fbx_format(const std::string& path)
{
	auto ext = get_ext(path);
	return is_fbx_format(utl::const_hash(ext.c_str(), ext.size()));
}

bool fs::is_skybox_format(const std::string& path)
{
	auto ext = get_ext(path);
	return is_skybox_format(utl::const_hash(ext.c_str(), ext.size()));
}

bool fs::is_script_format(const std::string& path)
{
	auto ext = get_ext(path);
	return is_script_format(utl::const_hash(ext.c_str(), ext.size()));
}

bool fs::is_audio_format(const std::string& path)
{
	auto ext = get_ext(path);
	return is_audio_format(utl::const_hash(ext.c_str(), ext.size()));
}

bool fs::is_particle_format(const std::string& path)
{
	auto ext = get_ext(path);
	return is_particle_format(utl::const_hash(ext.c_str(), ext.size()));
}

// Private
bool fs::is_scene_format(uint32_t format)
{
	return format == format::iescene;
}

bool fs::is_image_format(uint32_t format)
{
	return
		format == format::ktx ||
		format == format::ktx2 ||
		format == format::dds ||
		format == format::png ||
		format == format::jpg;
}

bool fs::is_ktx_format(uint32_t format)
{
	return
		format == format::ktx ||
		format == format::ktx2;
}

bool fs::is_mesh_format(uint32_t format)
{
	return
		format == format::gltf ||
		format == format::glb ||
		format == format::obj ||
		format == format::fbx;
}

bool fs::is_gltf_format(uint32_t format)
{
	return
		format == format::gltf ||
		format == format::glb;
}

bool fs::is_obj_format(uint32_t format)
{
	return format == format::obj;
}

bool fs::is_fbx_format(uint32_t format)
{
	return format == format::fbx;
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
	return format == format::js;
}

bool fs::is_audio_format(uint32_t format)
{
	return
		format == format::wav ||
		format == format::wave ||
		format == format::ogg;
}

bool fs::is_particle_format(uint32_t format)
{
	return format == format::efkefc;
}