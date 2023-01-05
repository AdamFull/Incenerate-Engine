#pragma once

#include <utility/uparse.hpp>

namespace engine
{
	namespace graphics
	{
		struct FProgramCreateInfo
		{
			std::string srStage{};
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
			FVertex(glm::vec3 p, glm::vec3 n, glm::vec2 t) : pos(p), normal(n), texcoord(t) {}

			glm::vec3 pos{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 texcoord{};
			glm::vec4 tangent{};

			bool operator==(const FVertex& other) const
			{
				return pos == other.pos && color == other.color && normal == other.normal && texcoord == other.texcoord && tangent == other.tangent;
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
				attributeDescriptions.resize(5);

				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
				attributeDescriptions[0].offset = offsetof(FVertex, pos);

				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
				attributeDescriptions[1].offset = offsetof(FVertex, color);

				attributeDescriptions[2].binding = 0;
				attributeDescriptions[2].location = 2;
				attributeDescriptions[2].format = vk::Format::eR32G32B32Sfloat;
				attributeDescriptions[2].offset = offsetof(FVertex, normal);

				attributeDescriptions[3].binding = 0;
				attributeDescriptions[3].location = 3;
				attributeDescriptions[3].format = vk::Format::eR32G32Sfloat;
				attributeDescriptions[3].offset = offsetof(FVertex, texcoord);

				attributeDescriptions[4].binding = 0;
				attributeDescriptions[4].location = 4;
				attributeDescriptions[4].format = vk::Format::eR32G32B32A32Sfloat;
				attributeDescriptions[4].offset = offsetof(FVertex, tangent);

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
			engine::graphics::hashCombine(seed, vertex.pos, vertex.color, vertex.normal, vertex.texcoord, vertex.tangent);
			return seed;
		}
	};
}