#pragma once

#include <utility/uparse.hpp>
#include <vulkan/vulkan.hpp>

namespace engine
{
	namespace graphics
	{
		struct FProgramCreateInfo
		{
			vk::PipelineBindPoint bindPoint;
			vk::CullModeFlagBits  cullMode;
			vk::FrontFace frontFace;
			bool depthTest{ false };
			std::vector<vk::DynamicState> dynamicStates;
			bool tesselation{ false };

			std::vector<std::string> stages;
			std::map<std::string, std::string> defines;
		};

		void to_json(nlohmann::json& json, const FProgramCreateInfo& type);
		void from_json(const nlohmann::json& json, FProgramCreateInfo& type);


		struct FCachedShader
		{
			vk::ShaderStageFlagBits shaderStage;

			std::vector<uint32_t> shaderCode;
			size_t hash{};

			bool operator==(const FCachedShader& rhs) const
			{
				return this->shaderStage == rhs.shaderStage && this->shaderCode == rhs.shaderCode;
			}

			bool operator!=(const FCachedShader& rhs) const
			{
				return this->shaderStage != rhs.shaderStage || this->shaderCode != rhs.shaderCode;
			}
		};

		void to_json(nlohmann::json& json, const FCachedShader& type);
		void from_json(const nlohmann::json& json, FCachedShader& type);
	}
}