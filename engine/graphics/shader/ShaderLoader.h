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

			std::unique_ptr<CShaderObject> load(const std::string& name);
		private:
			void load_config();
			void save_config();
		private:
			CDevice* pDevice{ nullptr };
			std::unique_ptr<CShaderCompiler> pCompiler;
			std::unordered_map<std::string, FProgramCreateInfo> programCI;
		};
	}
}