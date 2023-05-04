#pragma once

#include "APIStructures.h"

#include "EngineStructures.h"

namespace engine
{
	namespace filesystem { class IVirtualFileSystemInterface; }
	namespace graphics
	{
		class CShaderCompiler
		{
		public:
			CShaderCompiler(filesystem::IVirtualFileSystemInterface* vfs_ptr);
			~CShaderCompiler();

			std::optional<FCachedShader> compile(const std::string& path, const std::string& preamble, ERenderApi eAPI);

			std::optional<FCachedShader> get(const std::string& name, size_t hash);

		private:
			std::optional<FCachedShader> add(const std::string& name, vk::ShaderStageFlagBits stage, const std::vector<uint32_t>& code, size_t hash);
			std::optional<FCachedShader> update(const std::string& name, const std::vector<uint32_t>& code, size_t hash);

			void load_cache();
			void save_cache();

		private:
			std::unordered_map<std::string, FCachedShader> cache;
			filesystem::IVirtualFileSystemInterface* m_pVFS{ nullptr };
		};
	}
}