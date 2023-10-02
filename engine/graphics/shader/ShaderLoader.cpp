#include "ShaderLoader.h"

#include "Engine.h"
#include "graphics/rendering/material/Material.h"
#include "filesystem/vfs_helper.h"
#include <interface/filesystem/VirtualFileSystemInterface.h>
#include "pipeline/GraphicsPipeline.h"
#include "pipeline/ComputePipeline.h"

#include "APICompatibility.h"
#include <SessionStorage.hpp>

#include "../../../tools/shader_compiler/compiler_util.h"

using namespace engine::graphics;
using namespace engine::filesystem;

vk::ShaderStageFlagBits get_stage_flags(EShaderType type)
{
	switch (type)
	{
	case EShaderType::eVertex: return vk::ShaderStageFlagBits::eVertex; break;
	case EShaderType::eControl: return vk::ShaderStageFlagBits::eTessellationControl; break;
	case EShaderType::eEvaluation: return vk::ShaderStageFlagBits::eTessellationEvaluation; break;
	case EShaderType::eGeometry: return vk::ShaderStageFlagBits::eGeometry; break;
	case EShaderType::eFragment: return vk::ShaderStageFlagBits::eFragment; break;
	case EShaderType::eCompute: return vk::ShaderStageFlagBits::eCompute; break;
	case EShaderType::eRayGen: return vk::ShaderStageFlagBits::eRaygenKHR; break;
	case EShaderType::eIntersection: return vk::ShaderStageFlagBits::eIntersectionKHR; break;
	case EShaderType::eAnyHit: return vk::ShaderStageFlagBits::eAnyHitKHR; break;
	case EShaderType::eClosestHit: return vk::ShaderStageFlagBits::eClosestHitKHR; break;
	case EShaderType::eMiss: return vk::ShaderStageFlagBits::eMissKHR; break;
	case EShaderType::eCallable: return vk::ShaderStageFlagBits::eCallableKHR; break;
	case EShaderType::eTask: return vk::ShaderStageFlagBits::eTaskEXT; break;
	case EShaderType::eMesh: return vk::ShaderStageFlagBits::eMeshEXT; break;
	}

	return vk::ShaderStageFlagBits::eAll;
}

struct FShaderNameInfo
{
	std::string library_name;
	std::string shader_name;
	std::vector<std::string> definitions;
};

FShaderNameInfo get_shader_name_info(const std::string& shader_name)
{
	FShaderNameInfo name_info{};

	std::string token;
	std::stringstream ss(shader_name);

	if (std::getline(ss, token, ':'))
		name_info.library_name = token;

	if (std::getline(ss, token, ':'))
		name_info.shader_name = token;

	while (std::getline(ss, token, ':'))
		name_info.definitions.emplace_back(token);

	return name_info;
}

std::string make_shader_debug_name(const FShaderNameInfo& nameInfo)
{
	std::stringstream ss{};
	ss << nameInfo.library_name << ":" << nameInfo.shader_name << "(";

	size_t size = nameInfo.definitions.size();
	for (size_t i = 0ull; i < size; ++i)
		ss << nameInfo.definitions.at(i) << ((i < (size - 1)) ? "," : "");
	ss << ")";
	return ss.str();
}

size_t spirv_hash(const std::vector<uint32_t>& spirv)
{
	auto seed = spirv.size();
	for (auto x : spirv)
	{
		x = ((x >> 16) ^ x) * 0x45d9f3b;
		x = ((x >> 16) ^ x) * 0x45d9f3b;
		x = (x >> 16) ^ x;
		seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
	return seed;
}

size_t params_hash(const FShaderCreateInfo& params)
{
	std::stringstream ss;
	
	ss <<
		static_cast<uint32_t>(params.alpha_blend) <<
		static_cast<uint32_t>(params.bind_point) <<
		static_cast<uint32_t>(params.cull_mode) <<
		static_cast<uint32_t>(params.front_face) <<
		static_cast<uint32_t>(params.primitive_topology) <<
		static_cast<uint32_t>(params.vertex_type) <<
		params.depth_test <<
		params.double_sided <<
		params.use_bindles_textures <<
		params.pipeline_stage <<
		params.subpass <<
		params.usages;

	return std::hash<std::string>()(ss.str());
}

std::string shader_object_hash(const std::vector<uint32_t>& spirv, const FShaderCreateInfo& params)
{
	return std::to_string(spirv_hash(spirv)) + std::to_string(params_hash(params));
}

CShaderLoader::CShaderLoader(CDevice* device, IVirtualFileSystemInterface* vfs_ptr) :
	m_pDevice(device), m_pVFS(vfs_ptr)
{
	bEditorMode = CSessionStorage::getInstance()->get<bool>("editor_mode");
}

CShaderLoader::~CShaderLoader()
{
	m_pDevice = nullptr;
}

void CShaderLoader::create()
{
	auto directory_iterator = m_pVFS->walk("/embed/shader_libraries", true);
	for (auto& it = directory_iterator->begin(); it != directory_iterator->end(); ++it)
	{
		auto& current_path = *it;
		if (m_pVFS->isDirectory(current_path) || fs::get_ext(current_path) != ".shc")
			continue;

		FCompiledLibrary compiled_library{};
		if (m_pVFS->readBson(current_path, compiled_library))
		{
			auto library_name = fs::get_stem(current_path);

			if (shader_libraries.find(library_name) != shader_libraries.end())
				log_error("Shader library \"{}\" is already loaded. Trying to load it twice. Maybe something wrong with library name?");

			shader_libraries[library_name] = compiled_library;
		}
	}
}

std::string CShaderLoader::getShaderObjectCreateInfo(const std::string& shader_name, const FShaderCreateInfo& specials, FShaderObjectCreateInfo& module_ci)
{
	auto* graphics = m_pDevice->getAPI();

	std::vector<uint32_t> whole_code{};

	// Separate shader name to lib name, shader name and definitions
	auto name_info = get_shader_name_info(shader_name);

	// Add bindless texture support to shader
	if (specials.use_bindles_textures)
		name_info.definitions.emplace_back("BINDLESS_TEXTURES");

	// Add editor mode support to shader
	if (bEditorMode)
		name_info.definitions.emplace_back("EDITOR_MODE");

	module_ci.debug_shader_name = make_shader_debug_name(name_info);

	// Try to find shader library
	auto found_library = shader_libraries.find(name_info.library_name);
	if (found_library == shader_libraries.end())
		log_error("While loading shader \"{}\", library with name \"{}\" was not found.", shader_name, name_info.library_name);

	FCompiledLibrary& shader_library = found_library->second;

	// Try to find concrete shader modules
	auto shader_hashname = make_module_unique_name(name_info.shader_name, name_info.definitions);
	auto found_shader_module = shader_library.modules.find(shader_hashname);
	if(found_shader_module == shader_library.modules.end())
		log_error("While loading shader \"{}\", module with name \"{}\" was not found.", shader_name, name_info.shader_name);

	std::vector<FShaderModule>& shader_modules = found_shader_module->second;

	for (auto& module : shader_modules)
	{
		// Trying to find shader code
		auto pool_name = get_shader_type(module.type);
		auto found_shader_pool = shader_library.shaders.find(pool_name);
		if (found_shader_pool == shader_library.shaders.end())
		{
			log_error("While loading shader \"{}\", shader pool with name \"{}\" was not found.", shader_name, pool_name);
			continue;
		}

		auto& shader_pool = found_shader_pool->second;

		auto found_shader_code = shader_pool.find(module.link);
		if (found_shader_code == shader_pool.end())
		{
			log_error("While loading shader \"{}\", shader code with link \"{}\" was not found.", shader_name, module.link);
			continue;
		}

		module_ci.shader_modules.emplace_back();
		auto& shmodule = module_ci.shader_modules.back();

		shmodule.stage = get_stage_flags(module.type);
		shmodule.spirv = &found_shader_code->second;

		whole_code.insert(whole_code.end(), shmodule.spirv->begin(), shmodule.spirv->end());
	}

	return shader_object_hash(whole_code, specials);
}

std::unique_ptr<CShaderObject> CShaderLoader::load(const FShaderObjectCreateInfo& module_ci, const FShaderCreateInfo& specials)
{
	auto* graphics = m_pDevice->getAPI();

	auto pShaderObject = std::make_unique<CShaderObject>(m_pDevice);
	auto& pShader = pShaderObject->pShader;
	pShader->create(module_ci.debug_shader_name);

	for (auto& module : module_ci.shader_modules)
		pShader->addStage(*module.spirv, module.stage);

	pShader->buildReflection();

	pShaderObject->create(specials);

	return pShaderObject;
}