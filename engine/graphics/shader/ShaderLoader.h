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
			std::unique_ptr<CShaderObject> load(const std::string& name);

		private:
			CDevice* pDevice{ nullptr };
			std::unique_ptr<CShaderCompiler> pCompiler;
			std::unordered_map<std::string, FProgramCreateInfo> programCI;
		};
	}
}