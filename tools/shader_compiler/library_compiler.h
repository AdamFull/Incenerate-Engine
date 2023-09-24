#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>
#include <memory>
#include "shader_compiler.h"

class CLibraryCompiler
{
public:
	CLibraryCompiler() = default;
	CLibraryCompiler(const std::filesystem::path& common_path, const std::filesystem::path& includes_path);

	void initialize(ETargetAPIType target_api, ETargetBuildType build_type);
	void compile(const std::filesystem::path& library_path, const std::filesystem::path& library_out);
private:
	std::unique_ptr<CShaderCompiler> m_pShaderCompiler{};
	std::filesystem::path m_srCommonPath{};
	std::filesystem::path m_srIncludesPath{};
};