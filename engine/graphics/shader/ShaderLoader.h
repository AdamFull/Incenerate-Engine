#pragma once

#include "ShaderCompiler.h"
#include "ShaderObject.h"

namespace engine
{
	namespace filesystem { class IVirtualFileSystemInterface; }

	namespace graphics
	{
		struct FShaderSpecials
		{
			std::unordered_map<std::string, std::string> defines;
			uint32_t subpass{ 0 };
			size_t usages{ 1 };
			bool doubleSided{ false };
			EAlphaMode alphaBlend;
		};

		class CShaderLoader
		{
		public:
			CShaderLoader(CDevice* device, filesystem::IVirtualFileSystemInterface* vfs_ptr);
			~CShaderLoader();

			void create();
			std::unique_ptr<CShaderObject> load(const std::string& name, size_t mat_id);
			std::unique_ptr<CShaderObject> load(const std::string& name, const FShaderSpecials& specials);

		private:
			CDevice* m_pDevice{ nullptr };
			filesystem::IVirtualFileSystemInterface* m_pVFS{ nullptr };

			std::unique_ptr<CShaderCompiler> m_pCompiler;
			std::unordered_map<std::string, FProgramCreateInfo> programCI;
		};
	}
}