#pragma once

#include "ShaderCompiler.h"
#include "ShaderObject.h"

namespace engine
{
	namespace graphics
	{
		class CShaderLoader
		{
		public:
			CShaderLoader(CDevice* device);
			~CShaderLoader();

			void create(const std::string& srShaderPath);
			std::unique_ptr<CShaderObject> load(const std::string& name, size_t mat_id);
			std::unique_ptr<CShaderObject> load(const std::string& name, const std::map<std::string, std::string>& defines, uint32_t subpass = 0, size_t usages = 1);

		private:
			CDevice* pDevice{ nullptr };
			std::unique_ptr<CShaderCompiler> pCompiler;
			std::unordered_map<std::string, FProgramCreateInfo> programCI;
		};
	}
}