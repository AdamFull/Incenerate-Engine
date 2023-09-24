#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>

#include <logger/logger.h>

#include "library_compiler.h"

#include "compiler_util.h"

namespace fs = std::filesystem;

ETargetAPIType parse_api_type(const std::string& api_type_str)
{
	if (api_type_str == "vk10")
		return ETargetAPIType::eVK10;
	else if (api_type_str == "vk11")
		return ETargetAPIType::eVK11;
	else if (api_type_str == "vk12")
		return ETargetAPIType::eVK12;
	else if (api_type_str == "vk13")
		return ETargetAPIType::eVK13;
	else if (api_type_str == "gl")
		return ETargetAPIType::eGL;

	return ETargetAPIType::eVK10;
}

ETargetBuildType parse_build_type(const std::string& build_type_str)
{
	if (build_type_str == "release")
		return ETargetBuildType::eRelease;
	else if (build_type_str == "debug")
		return ETargetBuildType::eDebug;

	return ETargetBuildType::eRelease;
}

class CArgumentParser
{
public:
	CArgumentParser(int& argc, char** argv)
	{
		for (int i = 1; i < argc; ++i)
			this->tokens.push_back(std::string(argv[i]));
	}

	template<class _Ty>
	_Ty try_get(const std::string& option, _Ty default_value)
	{
		if (!exists(option))
			return default_value;

		auto svalue = get(option).value();

		if constexpr (std::is_same_v<_Ty, std::string>)
			return svalue;

		_Ty value;
		std::stringstream ss;
		ss << svalue;
		ss >> value;

		return value;
	}

	std::optional<std::string> get(const std::string& option) const
	{
		std::vector<std::string>::const_iterator itr;
		itr = std::find(this->tokens.begin(), this->tokens.end(), option);
		if (itr != this->tokens.end() && ++itr != this->tokens.end())
			return *itr;
		return std::nullopt;
	}

	bool exists(const std::string& option) const
	{
		return std::find(this->tokens.begin(), this->tokens.end(), option) != this->tokens.end();
	}
private:
	std::vector<std::string> tokens;
};

fs::path make_current_library_output_path(const fs::path& base_path, const fs::path& base_out_path, const fs::path& current_path)
{
	fs::path out_path{};
	if (base_out_path.empty())
		out_path = current_path;

	auto relative = fs::relative(current_path, base_path);
	return fs::weakly_canonical(base_out_path / relative);
}

int main(int argc, char** argv)
{
	log_add_file_output("shader_compiler.log");
	log_add_cout_output();
	log_init("shader_compiler", "1.0.0");

	CArgumentParser argparse(argc, argv);

	fs::path input_path{};
	fs::path compiled_libraries_output{};
	fs::path shaders_common{};
	fs::path shaders_include{};

	ETargetAPIType target_api{ ETargetAPIType::eVK10 };
	ETargetBuildType build_type{ ETargetBuildType::eRelease };

	input_path = argparse.try_get("-in", input_path);
	if (input_path.empty())
	{
		log_error("Input path is not set.");
		return -1;
	}

	compiled_libraries_output = argparse.try_get("-out", compiled_libraries_output);
	if (compiled_libraries_output.empty())
	{
		log_error("Output path is not set.");
		return -1;
	}

	shaders_common = argparse.try_get("-common", shaders_common);
	if (shaders_common.empty())
	{
		log_error("Common shaders path is not set.");
		return -1;
	}

	shaders_include = argparse.try_get("-includes", shaders_include);
	if (shaders_common.empty())
	{
		log_error("Common shaders path is not set.");
		return -1;
	}

	std::string target_api_str{ "vk10" };
	target_api_str = argparse.try_get("-api", target_api_str);
	target_api = parse_api_type(target_api_str);

	std::string build_type_str{ "release" };
	build_type_str = argparse.try_get("-build_type", build_type_str);
	build_type = parse_build_type(build_type_str);

	// Initialize library compiler
	auto library_compiler = std::make_unique<CLibraryCompiler>(shaders_common, shaders_include);
	library_compiler->initialize(target_api, build_type);

	// Start compilation process
	if (std::filesystem::is_regular_file(input_path))
	{
		auto file_extension = input_path.extension();
		bool is_library_file = file_extension == ".shlib";

		if (file_extension != ".shlib")
		{
			log_error("Trying to compile file with unsupported extension. File extension is \"{}\".", file_extension.string());
			return -1;
		}

		auto library_out = make_current_library_output_path(input_path.parent_path(), compiled_libraries_output, input_path.parent_path());
		if (!fs::exists(library_out))
			fs::create_directories(library_out);

		library_compiler->compile(input_path, library_out);
	}
	else
	{
		for (const auto& dir_entry : fs::recursive_directory_iterator(input_path))
		{
			if (dir_entry.is_directory())
				continue;

			auto& current_path = dir_entry.path();
			auto file_extension = current_path.extension();
			if (file_extension != ".shlib")
				continue;

			auto library_out = make_current_library_output_path(input_path, compiled_libraries_output, current_path.parent_path());
			if (!fs::exists(library_out))
				fs::create_directories(library_out);

			library_compiler->compile(current_path, library_out);
		}
	}

	return 0;
}