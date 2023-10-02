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

		struct FShaderObjectCreateInfo
		{
			std::string debug_shader_name{};
			std::vector<FShaderModuleCreateInfo> shader_modules{};
		};

		class CShaderLoader
		{
		public:
			CShaderLoader(CDevice* device, filesystem::IVirtualFileSystemInterface* vfs_ptr);
			~CShaderLoader();

			void create();
			std::string getShaderObjectCreateInfo(const std::string& shader_name, const FShaderCreateInfo& specials, FShaderObjectCreateInfo& module_ci);
			std::unique_ptr<CShaderObject> load(const FShaderObjectCreateInfo& module_ci, const FShaderCreateInfo& specials);

		private:
			CDevice* m_pDevice{ nullptr };
			filesystem::IVirtualFileSystemInterface* m_pVFS{ nullptr };

			std::unordered_map<std::string, FCompiledLibrary> shader_libraries{};

			bool bEditorMode{ false };
		};
	}
}