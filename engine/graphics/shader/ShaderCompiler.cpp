#include "ShaderCompiler.h"

#include <utility/logger/logger.h>
#include <utility/upattern.hpp>

#include "system/filesystem/filesystem.h"

#include <glslanglib/SPIRV/GlslangToSpv.h>
#include <glslanglib/glslang/Public/ShaderLang.h>

using namespace engine;
using namespace engine::graphics;
using namespace engine::system;

constexpr const char* cache_file_name = "spirv.cache";

class CShaderIncluder : public glslang::TShader::Includer
{
public:
	IncludeResult* includeLocal(const char* headerName, const char* includerName, size_t inclusionDepth) override
	{
		std::filesystem::path local_path{};
		if (inclusionDepth == 1)
		{
			directory = std::filesystem::path(includerName).parent_path();
			local_path = directory;
		}
		else
		{
			directory = directory / std::filesystem::path(includerName).parent_path();
			local_path = directory;
		}

		local_path = std::filesystem::weakly_canonical(directory / headerName);

		std::string fileLoaded;
		if (!fs::read_file(local_path, fileLoaded, true))
		{
			std::stringstream ss;
			ss << "In shader file: " << includerName << " Shader Include could not be loaded: " << std::quoted(headerName);
			log_error(ss.str());
			return nullptr;
		}

		auto content = new char[fileLoaded.size()];
		std::memcpy(content, fileLoaded.data(), fileLoaded.size());
		return new IncludeResult(headerName, content, fileLoaded.size(), content);
	}

	IncludeResult* includeSystem(const char* headerName, const char* includerName, size_t inclusionDepth) override
	{
		auto header = std::filesystem::path("shaders") / headerName;

		std::string fileLoaded;
		if (!fs::read_file(header, fileLoaded, true)) {
			std::stringstream ss;
			ss << "Shader Include could not be loaded: " << std::quoted(headerName);
			log_error(ss.str());
			return nullptr;
		}

		auto content = new char[fileLoaded.size()];
		std::memcpy(content, fileLoaded.data(), fileLoaded.size());
		return new IncludeResult(headerName, content, fileLoaded.size(), content);
	}

	void releaseInclude(IncludeResult* result) override
	{
		if (result)
		{
			delete[] result->userData;
			delete result;
		}
	}
private:
	std::filesystem::path directory{ "" };
};

vk::ShaderStageFlagBits getShaderStage(const std::filesystem::path& moduleName)
{
	auto fileExt = fs::get_ext(moduleName);
	std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), ::tolower);

	if (fileExt == ".comp")
		return vk::ShaderStageFlagBits::eCompute;
	if (fileExt == ".vert")
		return vk::ShaderStageFlagBits::eVertex;
	if (fileExt == ".tesc")
		return vk::ShaderStageFlagBits::eTessellationControl;
	if (fileExt == ".tese")
		return vk::ShaderStageFlagBits::eTessellationEvaluation;
	if (fileExt == ".geom")
		return vk::ShaderStageFlagBits::eGeometry;
	if (fileExt == ".frag")
		return vk::ShaderStageFlagBits::eFragment;
	if (fileExt == ".rchit")
		return vk::ShaderStageFlagBits::eClosestHitKHR;
	if (fileExt == ".rmiss")
		return vk::ShaderStageFlagBits::eMissKHR;
	if (fileExt == ".rgen")
		return vk::ShaderStageFlagBits::eRaygenKHR;
	return vk::ShaderStageFlagBits::eAll;
}

EShLanguage getEshLanguage(vk::ShaderStageFlagBits stageFlag)
{
	switch (stageFlag) {
	case vk::ShaderStageFlagBits::eVertex:
		return EShLangVertex;
	case vk::ShaderStageFlagBits::eTessellationControl:
		return EShLangTessControl;
	case vk::ShaderStageFlagBits::eTessellationEvaluation:
		return EShLangTessEvaluation;
	case vk::ShaderStageFlagBits::eGeometry:
		return EShLangGeometry;
	case vk::ShaderStageFlagBits::eFragment:
		return EShLangFragment;
	case vk::ShaderStageFlagBits::eCompute:
		return EShLangCompute;
	case vk::ShaderStageFlagBits::eRaygenKHR:
		return EShLangRayGen;
	case vk::ShaderStageFlagBits::eIntersectionKHR:
		return EShLangIntersect;
	case vk::ShaderStageFlagBits::eAnyHitKHR:
		return EShLangAnyHit;
	case vk::ShaderStageFlagBits::eClosestHitKHR:
		return EShLangClosestHit;
	case vk::ShaderStageFlagBits::eMissKHR:
		return EShLangMiss;
	case vk::ShaderStageFlagBits::eCallableKHR:
		return EShLangCallable;
	case vk::ShaderStageFlagBits::eTaskNV:
		return EShLangTaskNV;
	case vk::ShaderStageFlagBits::eMeshNV:
		return EShLangMeshNV;
	default:
		return EShLangCount;
	}
}

TBuiltInResource getResources()
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

glslang::EShTargetClientVersion getClientVersion(ERenderApi eAPI)
{
	switch (eAPI)
	{
	case ERenderApi::eVulkan_1_0:
		return glslang::EShTargetVulkan_1_0;
	case ERenderApi::eVulkan_1_1:
		return glslang::EShTargetVulkan_1_1;
	case ERenderApi::eVulkan_1_2:
		return glslang::EShTargetVulkan_1_2;
	case ERenderApi::eVulkan_1_3:
		return glslang::EShTargetVulkan_1_3;
	default:
		break;
	}
}

glslang::EShClient getClient(ERenderApi eAPI)
{
	switch (eAPI)
	{
	case ERenderApi::eVulkan_1_0:
	case ERenderApi::eVulkan_1_1:
	case ERenderApi::eVulkan_1_2:
	case ERenderApi::eVulkan_1_3:
		return glslang::EShClientVulkan;
	default:
		return glslang::EShClientNone;
	}
}

CShaderCompiler::CShaderCompiler()
{
	load_cache();

	if (!glslang::InitializeProcess())
		log_error("Failed to initialize glslang processor.");
}

CShaderCompiler::~CShaderCompiler()
{
	glslang::FinalizeProcess();
}

std::optional<FCachedShader> CShaderCompiler::compile(const std::filesystem::path& path, const std::string& preamble, ERenderApi eAPI)
{
	// TODO: shader caching works wrong
	std::string data;
	if (fs::read_file(path, data, true))
	{
		auto fname = fs::get_filename(path);
		auto hash = utl::const_hash(data.c_str());
		//auto found = get(fname, hash);
		//if (found)
		//	return found;

		auto stage = getShaderStage(path);
		auto language = getEshLanguage(stage);

		glslang::TProgram program;
		glslang::TShader shader(language);
		auto resources = getResources();

		auto messages = static_cast<EShMessages>(EShMsgSpvRules | EShMsgVulkanRules | EShMsgDefault);

		auto srShaderName = fs::from_unicode(path);
		auto shaderName = srShaderName.c_str();
		auto shaderSource = data.c_str();
		shader.setStringsWithLengthsAndNames(&shaderSource, nullptr, &shaderName, 1);
		shader.setPreamble(preamble.c_str());

		auto clientVersion = getClientVersion(eAPI);
		auto client = getClient(eAPI);
		shader.setEnvInput(glslang::EShSourceGlsl, language, client, 110);
		shader.setEnvClient(client, clientVersion);
		shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_3); //glslang::EShTargetSpv_1_3
		// vk1.1 - spirv1.3
		// vk1.2 - spirv1.5
		// vk1.3 - spirv1.6

		std::string str{};
		CShaderIncluder includer;

		if (!shader.preprocess(&resources, clientVersion, ENoProfile, false, false, messages, &str, includer))
		{
			std::stringstream ss;
			ss << "\n****************SHADER_PREPROCESSING_FAILED****************\n";
			ss << "Shader: " << path << "\n";
			ss << shader.getInfoLog() << "\n";
			ss << shader.getInfoDebugLog() << "\n";
			ss << "***********************************************************\n";
			ss << "SPRIV shader parse failed!";
			auto msg = ss.str();
			log_error(msg);
		}

		if (!shader.parse(&resources, clientVersion, true, messages, includer))
		{
			std::stringstream ss;
			ss << "\n*******************SHADER_PARSING_FAILED*******************\n";
			ss << "Shader: " << path << "\n";
			ss << shader.getInfoLog() << "\n";
			ss << shader.getInfoDebugLog() << "\n";
			ss << "***********************************************************\n";
			ss << "SPRIV shader parse failed!";
			auto msg = ss.str();
			log_error(msg);
		}

		program.addShader(&shader);

		if (!program.link(messages) || !program.mapIO())
		{
			std::stringstream ss;
			ss << "\n*******************SHADER_LINKING_FAILED*******************\n";
			ss << "Shader: " << path << "\n";
			ss << shader.getInfoLog() << "\n";
			ss << shader.getInfoDebugLog() << "\n";
			ss << "***********************************************************\n";
			ss << "Error while linking shader program.";
			auto msg = ss.str();
			log_error(msg);
		}

		glslang::SpvOptions spvOptions;
#if defined(_DEBUG)
		spvOptions.generateDebugInfo = true;
		spvOptions.disableOptimizer = true;
		spvOptions.optimizeSize = false;
#else
		spvOptions.generateDebugInfo = false;
		spvOptions.disableOptimizer = false;
		spvOptions.optimizeSize = true;
#endif

		spv::SpvBuildLogger logger;

		std::vector<uint32_t> spirv;
		GlslangToSpv(*program.getIntermediate(static_cast<EShLanguage>(language)), spirv, &logger, &spvOptions);

		//return add(fname, stage, spirv, hash);
		return FCachedShader{ stage, spirv, hash };
	}

	return std::nullopt;
}

std::optional<FCachedShader> CShaderCompiler::get(const std::string& name, size_t hash)
{
	auto found = cache.find(name);
	if (found != cache.end())
	{
		if (hash != found->second.hash)
			return std::nullopt;
		return found->second;
	}
	return std::nullopt;
}

std::optional<FCachedShader> CShaderCompiler::add(const std::string& name, vk::ShaderStageFlagBits stage, const std::vector<uint32_t>& code, size_t hash)
{
	auto it = cache.find(name);
	if (it != cache.end())
	{
		if (hash != it->second.hash)
			return update(name, code, hash);
	}

	cache.emplace(name, FCachedShader{ stage, code, hash });
	save_cache();

	return cache[name];
}

std::optional<FCachedShader> CShaderCompiler::update(const std::string& name, const std::vector<uint32_t>& code, size_t hash)
{
	cache[name].shaderCode = code;
	cache[name].hash = hash;
	save_cache();

	return cache[name];
}

void CShaderCompiler::load_cache()
{
	fs::read_json(cache_file_name, cache, true);
}

void CShaderCompiler::save_cache()
{
	fs::write_json(cache_file_name, cache, -1, true);
}