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
			std::string getShaderCreateInfo(const std::string& shaderName, size_t mat_id, std::vector<std::optional<FCachedShader>>& shaderCode, FPipelineParams& pipelineParams);
			std::string getShaderCreateInfo(const std::string& shaderName, const FShaderSpecials& specials, std::vector<std::optional<FCachedShader>>& shaderCode, FPipelineParams& pipelineParams);
			std::unique_ptr<CShaderObject> load(const std::vector<std::optional<FCachedShader>>& shaderCode, const FPipelineParams& pipelineParams);

		private:
			CDevice* m_pDevice{ nullptr };
			filesystem::IVirtualFileSystemInterface* m_pVFS{ nullptr };

			std::unique_ptr<CShaderCompiler> m_pCompiler;
			std::unordered_map<std::string, FProgramCreateInfo> programCI;

			bool bEditorMode{ false };
			bool bBindlessFeature{ false };
		};
	}
}