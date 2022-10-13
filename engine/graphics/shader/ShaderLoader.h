#pragma once

#include "ShaderCompiler.h"
#include "ShaderObject.h"

#include <utility/upattern.hpp>

namespace engine
{
	namespace graphics
	{
		class CShaderLoader : public utl::singleton<CShaderLoader>
		{
		protected:
			CShaderLoader();
		public:
			friend std::unique_ptr<CShaderLoader> std::make_unique<CShaderLoader>();
			~CShaderLoader();

			std::unique_ptr<CShaderObject> load(const std::string& name);
		private:
			void load_config();
			void save_config();
		private:
			std::unique_ptr<CShaderCompiler> pCompiler;
			std::unordered_map<std::string, FProgramCreateInfo> programCI;
		};
	}
}