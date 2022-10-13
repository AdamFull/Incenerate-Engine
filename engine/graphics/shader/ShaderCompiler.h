#pragma once

#include "APIStructures.h"

#include <filesystem>

#include "EngineEnums.h"

namespace engine
{
	namespace graphics
	{
		class CShaderCompiler
		{
		public:
			CShaderCompiler();
			~CShaderCompiler();

			std::optional<FCachedShader> compile(const std::filesystem::path& path, const std::string& preamble, ERenderApi eAPI);

			std::optional<FCachedShader> get(const std::string& name, size_t hash);

		private:
			std::optional<FCachedShader> add(const std::string& name, vk::ShaderStageFlagBits stage, const std::vector<uint32_t>& code, size_t hash);
			std::optional<FCachedShader> update(const std::string& name, const std::vector<uint32_t>& code, size_t hash);

			void load_cache();
			void save_cache();

		private:
			std::map<std::string, FCachedShader> cache;
		};
	}
}