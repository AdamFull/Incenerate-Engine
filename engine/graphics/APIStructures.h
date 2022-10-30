#pragma once

#include <utility/uparse.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

namespace engine
{
	namespace graphics
	{
		struct FProgramCreateInfo
		{
			bool vertexfree{ false };
			vk::PipelineBindPoint bindPoint;
			vk::PrimitiveTopology topology;
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

		
		struct FVertex
		{
			FVertex() = default;
			FVertex(glm::vec3 pos, uint32_t color) : pos(pos), color(color) {}
			glm::vec3 pos{0.f};
			uint32_t color;

			bool operator==(const FVertex& other) const
			{
				return pos == other.pos && color == other.color;
			}

			static vk::VertexInputBindingDescription getBindingDescription()
			{
				vk::VertexInputBindingDescription bindingDescription = {};
				bindingDescription.binding = 0;
				bindingDescription.stride = sizeof(FVertex);
				bindingDescription.inputRate = vk::VertexInputRate::eVertex;

				return bindingDescription;
			}

			static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions()
			{
				std::vector<vk::VertexInputAttributeDescription> attributeDescriptions = {};
				attributeDescriptions.resize(2);

				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
				attributeDescriptions[0].offset = offsetof(FVertex, pos);

				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = vk::Format::eR8G8B8A8Unorm;
				attributeDescriptions[1].offset = offsetof(FVertex, color);

				return attributeDescriptions;
			}
		};

		template <typename T, typename... Rest>
		void hashCombine(std::size_t& seed, const T& v, const Rest &...rest)
		{
			seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
			(hashCombine(seed, rest), ...);
		};
	}
}

namespace std
{
	template <>
	struct hash<engine::graphics::FVertex>
	{
		size_t operator()(engine::graphics::FVertex const& vertex) const
		{
			size_t seed = 0;
			engine::graphics::hashCombine(seed, vertex.pos, vertex.color);
			return seed;
		}
	};
}