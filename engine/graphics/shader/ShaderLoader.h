#pragma once

#include "ShaderObject.h"
#include "../../../tools/shader_compiler/shader_library_parse.hpp"

namespace engine
{
	namespace filesystem { class IVirtualFileSystemInterface; }

	namespace graphics
	{
		struct FShaderModuleCreateInfo
		{
			vk::ShaderStageFlagBits stage;
			std::vector<uint32_t>* spirv{ nullptr };
		};

		class CShaderLoader
		{
		public:
			CShaderLoader(CDevice* device, filesystem::IVirtualFileSystemInterface* vfs_ptr);
			~CShaderLoader();

			void create();
			std::string getShaderCreateInfo(const std::string& shader_name, const FShaderCreateInfo& specials, std::vector<FShaderModuleCreateInfo>& module_ci);
			std::unique_ptr<CShaderObject> load(const std::vector<FShaderModuleCreateInfo>& module_ci, const FShaderCreateInfo& specials);

		private:
			CDevice* m_pDevice{ nullptr };
			filesystem::IVirtualFileSystemInterface* m_pVFS{ nullptr };

			std::unordered_map<std::string, FCompiledLibrary> shader_libraries{};

			bool bEditorMode{ false };
		};
	}
}