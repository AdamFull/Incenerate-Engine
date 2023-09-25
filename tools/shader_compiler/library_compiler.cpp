#include "library_compiler.h"

#include "compiler_util.h"
#include "compiler_parse.h"
#include "shader_library_parse.hpp"
#include "sjson_parser.h"

#include <fstream>

#include <logger/logger.h>
#include <json.hpp>

std::string emplace_data(std::string base_string, const std::string& target, const std::string& replacement)
{
	size_t start_pos = base_string.find(target);
	if (start_pos == std::string::npos)
		return base_string;
	base_string.replace(start_pos, target.length(), replacement);
	return base_string;
}

template<class _Ty>
bool parse_file_to(const std::filesystem::path& file_path, _Ty& output)
{
	if (!std::filesystem::exists(file_path))
	{
		log_error("File {} was not found.", file_path.string());
		return false;
	}

	std::ifstream file(file_path, std::ios_base::in | std::ios_base::binary);
	if (!file.is_open())
	{
		log_error("Failed to open file: {}.", file_path.string());
		return false;
	}

	std::string file_data{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
	if (file_data.empty())
	{
		log_warning("File loaded from: {} is empty.", file_path.string());
		return false;
	}

	sjson::parse(file_data).parse_to<_Ty>(output);

	return true;
}

template<class _Ty>
bool parse_json_to(const std::filesystem::path& file_path, _Ty& output)
{
	if (!std::filesystem::exists(file_path))
		return false;

	std::ifstream file(file_path, std::ios_base::in | std::ios_base::binary);
	if (!file.is_open())
		return false;

	std::string file_data{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
	if (file_data.empty())
		return false;

	nlohmann::json::from_bson(file_data).get_to(output);
}

template<class _Ty>
bool dump_json_from(const std::filesystem::path& file_path, const _Ty& input)
{
	std::ofstream file(file_path, std::ios_base::out | std::ios_base::binary);
	if (!file.is_open())
		return false;

	auto data = nlohmann::json::to_bson(nlohmann::json(input));
	file.write(reinterpret_cast<const char*>(data.data()), data.size());
	file.close();
	
	return true;
}

std::vector<std::string> make_permutation(size_t mask, size_t total, const std::vector<std::string>& source)
{
	std::vector<std::string> permutation;
	for (size_t j = 0; j < total; ++j)
	{
		if (mask & (1 << j))
			permutation.emplace_back(source[j]);
	}
	return permutation;
}

std::string generate_shader_code(const FShaderObject& sobject, const std::filesystem::path& includes_path, size_t depth = 0)
{
	std::stringstream ss{};

	if (depth == 0ull)
	{
		// Set language version
		ss << std::format("#version {}", sobject.version) << std::endl << std::endl;

		// Apply enabled extensions
		for (auto& extension_name : sobject.enable_extensions)
			ss << std::format("#extension {} : enable", extension_name) << std::endl;
		ss << std::endl;

		// Apply required extensions
		for (auto& extension_name : sobject.require_extensions)
			ss << std::format("#extension {} : require", extension_name) << std::endl;
		ss << std::endl;

		// Apply definitions (TODO: add definition values)
		for (auto& definition : sobject.definitions)
			ss << std::format("#define {}", definition) << std::endl;

		if (depth == 0ull)
			ss << "{other_definitions}" << std::endl << std::endl; // For other definitions
	}

	// Include 
	for (auto& sinclude : sobject.includes)
	{
		auto include_path = includes_path / (sinclude + ".shinc");
		FShaderObject include_so{};
		if (!parse_file_to(include_path, include_so))
			continue;

		auto protector_name = std::format("{}_protector", include_path.stem().string());

		ss << std::format("\n#ifndef {}\n#define {}", protector_name, protector_name) << std::endl;
		ss << generate_shader_code(include_so, includes_path, depth + 1);
		ss << "\n#endif" << std::endl << std::endl;
	}

	// Apply code
	ss << sobject.code;

	return ss.str();
}

CLibraryCompiler::CLibraryCompiler(const std::filesystem::path& common_path, const std::filesystem::path& includes_path)
{
	m_srCommonPath = common_path;
	m_srIncludesPath = includes_path;
}


void CLibraryCompiler::initialize(ETargetAPIType target_api, ETargetBuildType build_type)
{
	m_pShaderCompiler = std::make_unique<CShaderCompiler>(target_api, build_type);
	m_pShaderCompiler->initialize();
}

void CLibraryCompiler::compile(const std::filesystem::path& library_path, const std::filesystem::path& library_out)
{
	FCompiledLibrary compiled_library{};

	FShaderLibrary shader_library_ci{};
	if (!parse_file_to(library_path, shader_library_ci))
		return;

	auto out_library_path = library_out / (shader_library_ci.name + ".shc");
	if (!parse_json_to(out_library_path, compiled_library))
		log_info("Compiling library {} first time.", shader_library_ci.name);

	auto parent_path = library_path.parent_path();

	// Process compile all instructions from library file
	for (auto& compile : shader_library_ci.compiles)
	{
		auto found_shader_desc = shader_library_ci.shaders.find(compile.name);
		if (found_shader_desc == shader_library_ci.shaders.end())
		{
			log_error("Trying to compile shader {}, but shader description for this shader was not declared in \"shaders\" scope.", compile.name);
			continue;
		}

		FShaderDescription& shader_desc = found_shader_desc->second;

		if (compile.name == "tonemap")
			int iii = 0;

		// Generate all shader permutations
		bool generate_permutations = !compile.permutation_gen.definitions.empty();
		size_t perm_n = compile.permutation_gen.definitions.size();
		size_t total_permutations = 1 << perm_n;

		std::vector<std::vector<std::string>> cached_permutations{};

		if (generate_permutations)
		{
			for (size_t mask = 0; mask < total_permutations; ++mask)
				cached_permutations.emplace_back(make_permutation(mask, perm_n, compile.permutation_gen.definitions));
		}

		for (auto& permutation : compile.permutations)
			cached_permutations.emplace_back(permutation.definitions);

		// Process all shader included sources
		for (auto& source : shader_desc.sources)
		{
			// TODO: i think i should create common library that will contatin common sources
			auto shader_path = (source.common ? m_srCommonPath : parent_path) / (source.shader_name + ".shade");

			// Parse shader file
			FShaderObject shader_object{};
			if (!parse_file_to(shader_path, shader_object))
				continue;

			// Generate shader code (apply extensions, process includes and definitions)
			auto base_shader_code = generate_shader_code(shader_object, m_srIncludesPath);
			auto shader_type_string = get_shader_type(source.type);
			auto& shader_pool = compiled_library.shaders[shader_type_string];
			
			// Process and compile all definition combinations
			for (auto& definitions : cached_permutations)
			{
				FCompileInfo compile_info{ source.type, source.shader_name };

				// Make unique shader name(key)
				auto unique_module_name = make_module_unique_name(compile.name, definitions);

				// Get shader module
				std::vector<FShaderModule>& shader_modules = compiled_library.modules[unique_module_name];

				// Apply definitions for current permutation
				std::stringstream ss{""};
				for (auto& definition : definitions)
					ss << std::format("#define {}", definition) << std::endl;
				ss << std::endl;

				auto shader_code = emplace_data(base_shader_code, "{other_definitions}", ss.str());

				// Preprocess generated code (and remove ugly code after preprocessing)
				std::string preprocessed_code{};
				size_t preprocessed_hash{ 0ull };
				m_pShaderCompiler->preprocess(shader_code, compile_info, preprocessed_code, preprocessed_hash);

				if (preprocessed_code.empty())
				{
					log_error("Something went wrong while preprocessing code. Empty string after preprocessing.");
					return; // throw error here
				}

				// Make hash for preprocessed data
				auto hash_string = make_shader_unique_name(preprocessed_hash);
				auto& compiled_spirv = shader_pool[hash_string];

				bool is_compiling_first_time{ false };
				FShaderModule* shader_module{ nullptr };

				// Find existing shader or create new
				auto found_module = std::find_if(shader_modules.begin(), shader_modules.end(), 
					[type = source.type, link = hash_string](const FShaderModule& sm) 
					{ 
						return (sm.type == type) && (sm.link == link);
					});
				if (found_module != shader_modules.end())
					shader_module = &(*found_module);
				else
				{
					shader_modules.emplace_back();
					shader_module = &shader_modules.back();
					is_compiling_first_time = true;
				}

				shader_module->type = source.type;

				// Compile shader if we need
				bool need_to_compile = is_compiling_first_time ? (compiled_spirv.empty()) : (shader_module->link != hash_string);
				if (need_to_compile)
				{
					compiled_spirv = m_pShaderCompiler->compile(preprocessed_code, compile_info);
					log_info("{} shader {}[{}] was {}.", shader_type_string, compile_info.name, hash_string, is_compiling_first_time ? "compiled" : "recompiled");
				}

				// Removing old shader data
				if (!shader_module->link.empty() && need_to_compile)
					shader_pool.erase(shader_module->link);

				shader_module->link = hash_string;
			}
		}
	}

	dump_json_from(out_library_path, compiled_library);
}