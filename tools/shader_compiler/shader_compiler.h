#pragma once

#include "compiler_structures.h"

struct FCompileInfo
{
	EShaderType type;
	std::string name;
};

class CShaderCompiler
{
public:
	CShaderCompiler(ETargetAPIType target_api, ETargetBuildType build_type);
	~CShaderCompiler();

	void initialize();

	void preprocess(const std::string& shader_code, const FCompileInfo& info, std::string& preprocessed_code, size_t& preprocessed_hash);
	std::vector<uint32_t> compile(const std::string& shader_code, const FCompileInfo& info);
private:
	ETargetBuildType build_type{ ETargetBuildType::eRelease };
	ETargetAPIType target_api{ ETargetAPIType::eVK13 };
};