#include "ShaderLoader.h"

#include "Engine.h"
#include "system/filesystem/filesystem.h"

constexpr const char* shader_config_path = "shaders.json";

using namespace engine::graphics;

CShaderLoader::CShaderLoader()
{
	load_config();

	pCompiler = std::make_unique<CShaderCompiler>();
}

CShaderLoader::~CShaderLoader()
{
	pCompiler = nullptr;
}

std::unique_ptr<CShaderObject> CShaderLoader::load(const std::string& name)
{
	auto it = programCI.find(name);
	if (it != programCI.end())
	{
		auto& gapi = CEngine::getInstance()->getGraphics();
		auto api = gapi->getAPI();
	}

	return nullptr;
}

void CShaderLoader::load_config()
{

}

void CShaderLoader::save_config()
{

}