#include "shader_compiler.h"

#include <SPIRV/GlslangToSpv.h>
#include <glslang/Public/ShaderLang.h>

#include <logger/logger.h>
#include <udetail.hpp>

#include <stack>

class empty_includer : public glslang::TShader::Includer
{
public:
	IncludeResult* includeLocal(const char* headerName, const char* includerName, size_t inclusionDepth) override { return nullptr; }
	IncludeResult* includeSystem(const char* headerName, const char* includerName, size_t inclusionDepth) override { return nullptr; }
	void releaseInclude(IncludeResult* result) override {}
};

EShLanguage get_sh_language(EShaderType type)
{
	switch (type)
	{
	case EShaderType::eVertex: return EShLangVertex; break;
	case EShaderType::eControl: return EShLangTessControl; break;
	case EShaderType::eEvaluation: return EShLangTessEvaluation; break;
	case EShaderType::eGeometry: return EShLangGeometry; break;
	case EShaderType::eFragment: return EShLangFragment; break;
	case EShaderType::eCompute: return EShLangCompute; break;
	case EShaderType::eRayGen: return EShLangRayGen; break;
	case EShaderType::eIntersection: return EShLangIntersect; break;
	case EShaderType::eAnyHit: return EShLangAnyHit; break;
	case EShaderType::eClosestHit: return EShLangClosestHit; break;
	case EShaderType::eMiss: return EShLangMiss; break;
	case EShaderType::eCallable: return EShLangCallable; break;
	case EShaderType::eTask: return EShLangTaskNV; break;
	case EShaderType::eMesh: return EShLangMeshNV; break;
	}

	return EShLangCount;
}

TBuiltInResource get_resources()
{
	//auto props = UDevice->GetPhysical().getProperties();
	TBuiltInResource resources = {};
	resources.maxLights = 32;
	resources.maxClipPlanes = 6;
	resources.maxTextureUnits = 32;
	resources.maxTextureCoords = 32;
	resources.maxVertexAttribs = 64;
	resources.maxVertexUniformComponents = 4096;
	resources.maxVaryingFloats = 64;
	resources.maxVertexTextureImageUnits = 32;
	resources.maxCombinedTextureImageUnits = 80;
	resources.maxTextureImageUnits = 32;
	resources.maxFragmentUniformComponents = 4096;
	resources.maxDrawBuffers = 32;
	resources.maxVertexUniformVectors = 128;
	resources.maxVaryingVectors = 8;
	resources.maxFragmentUniformVectors = 16;
	resources.maxVertexOutputVectors = 16;
	resources.maxFragmentInputVectors = 15;
	resources.minProgramTexelOffset = -8;
	resources.maxProgramTexelOffset = 7;
	resources.maxClipDistances = 8;
	resources.maxComputeWorkGroupCountX = 65535;
	resources.maxComputeWorkGroupCountY = 65535;
	resources.maxComputeWorkGroupCountZ = 65535;
	resources.maxComputeWorkGroupSizeX = 1024;
	resources.maxComputeWorkGroupSizeY = 1024;
	resources.maxComputeWorkGroupSizeZ = 64;
	resources.maxComputeUniformComponents = 1024;
	resources.maxComputeTextureImageUnits = 16;
	resources.maxComputeImageUniforms = 8;
	resources.maxComputeAtomicCounters = 8;
	resources.maxComputeAtomicCounterBuffers = 1;
	resources.maxVaryingComponents = 60;
	resources.maxVertexOutputComponents = 64;
	resources.maxGeometryInputComponents = 64;
	resources.maxGeometryOutputComponents = 128;
	resources.maxFragmentInputComponents = 128;
	resources.maxImageUnits = 8;
	resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
	resources.maxCombinedShaderOutputResources = 8;
	resources.maxImageSamples = 0;
	resources.maxVertexImageUniforms = 0;
	resources.maxTessControlImageUniforms = 0;
	resources.maxTessEvaluationImageUniforms = 0;
	resources.maxGeometryImageUniforms = 0;
	resources.maxFragmentImageUniforms = 8;
	resources.maxCombinedImageUniforms = 8;
	resources.maxGeometryTextureImageUnits = 16;
	resources.maxGeometryOutputVertices = 256;
	resources.maxGeometryTotalOutputComponents = 1024;
	resources.maxGeometryUniformComponents = 1024;
	resources.maxGeometryVaryingComponents = 64;
	resources.maxTessControlInputComponents = 128;
	resources.maxTessControlOutputComponents = 128;
	resources.maxTessControlTextureImageUnits = 16;
	resources.maxTessControlUniformComponents = 1024;
	resources.maxTessControlTotalOutputComponents = 4096;
	resources.maxTessEvaluationInputComponents = 128;
	resources.maxTessEvaluationOutputComponents = 128;
	resources.maxTessEvaluationTextureImageUnits = 16;
	resources.maxTessEvaluationUniformComponents = 1024;
	resources.maxTessPatchComponents = 120;
	resources.maxPatchVertices = 32;
	resources.maxTessGenLevel = 64;
	resources.maxViewports = 16;
	resources.maxVertexAtomicCounters = 0;
	resources.maxTessControlAtomicCounters = 0;
	resources.maxTessEvaluationAtomicCounters = 0;
	resources.maxGeometryAtomicCounters = 0;
	resources.maxFragmentAtomicCounters = 8;
	resources.maxCombinedAtomicCounters = 8;
	resources.maxAtomicCounterBindings = 1;
	resources.maxVertexAtomicCounterBuffers = 0;
	resources.maxTessControlAtomicCounterBuffers = 0;
	resources.maxTessEvaluationAtomicCounterBuffers = 0;
	resources.maxGeometryAtomicCounterBuffers = 0;
	resources.maxFragmentAtomicCounterBuffers = 1;
	resources.maxCombinedAtomicCounterBuffers = 1;
	resources.maxAtomicCounterBufferSize = 16384;
	resources.maxTransformFeedbackBuffers = 4;
	resources.maxTransformFeedbackInterleavedComponents = 64;
	resources.maxCullDistances = 8;
	resources.maxCombinedClipAndCullDistances = 8;
	resources.maxSamples = 4;
	resources.maxMeshOutputVerticesNV = 256;
	resources.maxMeshOutputPrimitivesNV = 512;
	resources.maxMeshWorkGroupSizeX_NV = 32;
	resources.maxMeshWorkGroupSizeY_NV = 1;
	resources.maxMeshWorkGroupSizeZ_NV = 1;
	resources.maxTaskWorkGroupSizeX_NV = 32;
	resources.maxTaskWorkGroupSizeY_NV = 1;
	resources.maxTaskWorkGroupSizeZ_NV = 1;
	resources.maxMeshViewCountNV = 4;

	resources.limits.nonInductiveForLoops = true;
	resources.limits.whileLoops = true;
	resources.limits.doWhileLoops = true;
	resources.limits.generalUniformIndexing = true;
	resources.limits.generalAttributeMatrixVectorIndexing = true;
	resources.limits.generalVaryingIndexing = true;
	resources.limits.generalSamplerIndexing = true;
	resources.limits.generalVariableIndexing = true;
	resources.limits.generalConstantMatrixVectorIndexing = true;
	return resources;
}

glslang::EShTargetClientVersion get_client_version(ETargetAPIType eAPI)
{
	switch (eAPI)
	{
	case ETargetAPIType::eVK10: return glslang::EShTargetVulkan_1_0; break;
	case ETargetAPIType::eVK11: return glslang::EShTargetVulkan_1_1; break;
	case ETargetAPIType::eVK12: return glslang::EShTargetVulkan_1_2; break;
	case ETargetAPIType::eVK13: return glslang::EShTargetVulkan_1_3; break;
	case ETargetAPIType::eGL: return glslang::EShTargetOpenGL_450; break;
	default:
		break;
	}
}

glslang::EShClient get_compile_client(ETargetAPIType eAPI)
{
	switch (eAPI)
	{
	case ETargetAPIType::eVK10:
	case ETargetAPIType::eVK11:
	case ETargetAPIType::eVK12:
	case ETargetAPIType::eVK13: return glslang::EShClientVulkan; break;
	case ETargetAPIType::eGL: return glslang::EShClientOpenGL; break;
	default: return glslang::EShClientNone;
	}
}

glslang::EShTargetLanguageVersion get_SPIR_version(ETargetAPIType eAPI)
{
	switch (eAPI)
	{
	case ETargetAPIType::eVK10: return glslang::EShTargetSpv_1_0; break;
	case ETargetAPIType::eVK11: return glslang::EShTargetSpv_1_3; break;
	case ETargetAPIType::eVK12: return glslang::EShTargetSpv_1_5; break;
	case ETargetAPIType::eVK13: return glslang::EShTargetSpv_1_6; break;
	default:
		return glslang::EShTargetSpv_1_0;
	}
}

std::string remove_spaces_around_dot(const std::string& line)
{
	std::string result;
	bool is_last_was_dot{ false };

	for (char c : line) 
	{
		if (c == ' ' && is_last_was_dot)
			continue;

		is_last_was_dot = (c == '.');
		result.push_back(c);
	}

	size_t pos = result.find(" .");
	while (pos != std::string::npos) 
	{
		result.erase(pos, 1);
		pos = result.find(" .", pos);
	}

	return result;
}

std::string clean_code(const std::string& input)
{
	std::istringstream stream(input);
	std::string cleaned_code, line;
	std::stack<char> block_stack;
	bool was_empty_line = false;

	while (std::getline(stream, line)) 
	{
		size_t first = line.find_first_not_of(" \t");
		if (first == std::string::npos) 
		{
			was_empty_line = true;
			continue;
		}

		size_t last = line.find_last_not_of(" \t");
		line = line.substr(first, (last - first + 1));

		line = remove_spaces_around_dot(line);

		if ((line.find('}') != std::string::npos) && !block_stack.empty())
			block_stack.pop();

		line = std::string(block_stack.size(), '\t') + line;

		if (line.find('{') != std::string::npos)
			block_stack.push('{');

		if (was_empty_line && !cleaned_code.empty())
			cleaned_code += '\n';
		cleaned_code += line + '\n';

		was_empty_line = false;
	}

	return cleaned_code;
}

CShaderCompiler::CShaderCompiler(ETargetAPIType target_api, ETargetBuildType build_type)
{
	this->target_api = target_api;
	this->build_type = build_type;
}

CShaderCompiler::~CShaderCompiler()
{
	glslang::FinalizeProcess();
}

void CShaderCompiler::initialize()
{
	if (!glslang::InitializeProcess())
		log_error("Failed to initialize glslang processor.");
}

void CShaderCompiler::preprocess(const std::string& shader_code, const FCompileInfo& info, std::string& preprocessed_code, size_t& preprocessed_hash)
{
	auto language = get_sh_language(info.type);
	glslang::TShader shader(language);

	auto resources = get_resources();

	auto messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules | EShMsgDefault);

	auto shader_source = shader_code.c_str();
	auto shader_name = info.name.c_str();
	shader.setStringsWithLengthsAndNames(&shader_source, nullptr, &shader_name, 1);

	auto client_version = get_client_version(target_api);
	auto compile_client = get_compile_client(target_api);
	auto spir_version = get_SPIR_version(target_api);
	shader.setEnvInput(glslang::EShSourceGlsl, language, compile_client, 110);
	shader.setEnvClient(compile_client, client_version);
	shader.setEnvTarget(glslang::EShTargetSpv, spir_version);

	empty_includer includer;
	std::string preprocessed_raw;
	if (!shader.preprocess(&resources, client_version, ENoProfile, false, false, messages, &preprocessed_raw, includer))
		log_error("\nPreprocessing error: In shader \"{}\" \n{}\n{}\n", info.name, shader.getInfoLog(), shader.getInfoDebugLog());

	preprocessed_code = clean_code(preprocessed_raw);

	preprocessed_hash = utl::fnv1a_64_hash(preprocessed_code.c_str());
}

std::vector<uint32_t> CShaderCompiler::compile(const std::string& shader_code, const FCompileInfo& info)
{
	std::vector<uint32_t> spirv{};

	auto language = get_sh_language(info.type);

	glslang::TProgram program;
	glslang::TShader shader(language);

	auto resources = get_resources();

	auto messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules | EShMsgDefault);

	auto shader_source = shader_code.c_str();
	auto shader_name = info.name.c_str();
	shader.setStringsWithLengthsAndNames(&shader_source, nullptr, &shader_name, 1);

	auto client_version = get_client_version(target_api);
	auto compile_client = get_compile_client(target_api);
	auto spir_version = get_SPIR_version(target_api);
	shader.setEnvInput(glslang::EShSourceGlsl, language, compile_client, 110);
	shader.setEnvClient(compile_client, client_version);
	shader.setEnvTarget(glslang::EShTargetSpv, spir_version);

	empty_includer includer;

	if (!shader.parse(&resources, client_version, true, messages, includer))
		log_error("\nParsing error: In shader \"{}\" \n{}\n{}\n", info.name, shader.getInfoLog(), shader.getInfoDebugLog());

	program.addShader(&shader);

	if (!program.link(messages) || !program.mapIO())
		log_error("\nLinking error: In shader \"{}\" \n{}\n{}\n", info.name, shader.getInfoLog(), shader.getInfoDebugLog());

	bool building_release = build_type == ETargetBuildType::eRelease;

	glslang::SpvOptions spvOptions;
	spvOptions.generateDebugInfo = !building_release;
	spvOptions.disableOptimizer = !building_release;
	spvOptions.optimizeSize = building_release;

	spv::SpvBuildLogger logger;
	GlslangToSpv(*program.getIntermediate(language), spirv, &logger, &spvOptions);

	std::string error{};
	logger.error(error);
	if(!error.empty())
		log_error("SPIRV generation error: In shader \"{}\" \n{}\n", info.name, error);

	std::string warning;
	logger.warning(warning);
	if(!warning.empty())
		log_error("SPIRV generation warnings: In shader \"{}\" \n{}\n", info.name, warning);

	return spirv;
}