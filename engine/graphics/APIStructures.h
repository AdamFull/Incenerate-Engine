#pragma once

#include <vulkan/vulkan.hpp>

namespace engine
{
	namespace graphics
	{
		enum class EAlphaMode
		{
			EOPAQUE,
			EMASK,
			EBLEND
		};

		enum class EVertexType
		{
			eNone,
			eDefault,
			eSmall
		};

		struct FShaderCreateInfo
		{
			std::string pipeline_stage{};
			EVertexType vertex_type{ EVertexType::eNone };
			vk::PipelineBindPoint bind_point{ vk::PipelineBindPoint::eGraphics };
			vk::PrimitiveTopology primitive_topology{ vk::PrimitiveTopology::eTriangleList };
			vk::CullModeFlags cull_mode{ vk::CullModeFlagBits::eNone };
			vk::FrontFace front_face{ vk::FrontFace::eClockwise };
			bool depth_test{ false };
			bool use_bindles_textures{ false };

			uint32_t subpass{ 0 };
			size_t usages{ 512 };
			bool double_sided{ false };
			EAlphaMode alpha_blend;
		};

		// For debug drawing
		struct FSimpleVertex
		{
			FSimpleVertex() = default;
			FSimpleVertex(glm::vec3 p, glm::vec3 c) : pos(p), color(c) {}

			glm::vec3 pos{};
			glm::vec3 color{};

			bool operator==(const FSimpleVertex& other) const
			{
				return
					pos == other.pos &&
					color == other.color;
			}

			static std::vector<vk::VertexInputBindingDescription> getBindingDescription()
			{
				std::vector<vk::VertexInputBindingDescription> bindingDescriptions{};
				bindingDescriptions.resize(2);

				bindingDescriptions[0].binding = 0;
				bindingDescriptions[0].stride = sizeof(glm::vec3);
				bindingDescriptions[0].inputRate = vk::VertexInputRate::eVertex;

				bindingDescriptions[1].binding = 1;
				bindingDescriptions[1].stride = sizeof(glm::vec3);
				bindingDescriptions[1].inputRate = vk::VertexInputRate::eVertex;

				return bindingDescriptions;
			}

			static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions()
			{
				std::vector<vk::VertexInputAttributeDescription> attributeDescriptions = {};
				attributeDescriptions.resize(2);

				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
				attributeDescriptions[0].offset = 0;

				attributeDescriptions[1].binding = 1;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
				attributeDescriptions[1].offset = 0;

				return attributeDescriptions;
			}
		};

		
		// For skeletal meshes with animation support
		struct FVertex
		{
			FVertex() = default;
			FVertex(glm::vec3 p, glm::vec3 c) : pos(p), color(c) {}
			FVertex(glm::vec3 p, glm::vec3 n, glm::vec2 t) : pos(p), normal(n), texcoord(t) {}

			glm::vec3 pos{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 texcoord{};
			glm::vec4 tangent{};
			glm::vec4 joint0{};
			glm::vec4 weight0{};

			bool operator==(const FVertex& other) const
			{
				return 
					pos == other.pos && 
					color == other.color && 
					normal == other.normal && 
					texcoord == other.texcoord && 
					tangent == other.tangent && 
					joint0 == other.joint0 && 
					weight0 == other.weight0;
			}

			static std::vector<vk::VertexInputBindingDescription> getBindingDescription()
			{
				std::vector<vk::VertexInputBindingDescription> bindingDescriptions{};
				bindingDescriptions.resize(7);

				bindingDescriptions[0].binding = 0;
				bindingDescriptions[0].stride = sizeof(glm::vec3);
				bindingDescriptions[0].inputRate = vk::VertexInputRate::eVertex;
				bindingDescriptions[1].binding = 1;
				bindingDescriptions[1].stride = sizeof(glm::vec3);
				bindingDescriptions[1].inputRate = vk::VertexInputRate::eVertex;
				bindingDescriptions[2].binding = 2;
				bindingDescriptions[2].stride = sizeof(glm::vec3);
				bindingDescriptions[2].inputRate = vk::VertexInputRate::eVertex;
				bindingDescriptions[3].binding = 3;
				bindingDescriptions[3].stride = sizeof(glm::vec2);
				bindingDescriptions[3].inputRate = vk::VertexInputRate::eVertex;
				bindingDescriptions[4].binding = 4;
				bindingDescriptions[4].stride = sizeof(glm::vec4);
				bindingDescriptions[4].inputRate = vk::VertexInputRate::eVertex;
				bindingDescriptions[5].binding = 5;
				bindingDescriptions[5].stride = sizeof(glm::vec4);
				bindingDescriptions[5].inputRate = vk::VertexInputRate::eVertex;
				bindingDescriptions[6].binding = 6;
				bindingDescriptions[6].stride = sizeof(glm::vec4);
				bindingDescriptions[6].inputRate = vk::VertexInputRate::eVertex;

				return bindingDescriptions;
			}

			static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions()
			{
				std::vector<vk::VertexInputAttributeDescription> attributeDescriptions = {};
				attributeDescriptions.resize(7);

				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
				attributeDescriptions[0].offset = 0;

				attributeDescriptions[1].binding = 1;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
				attributeDescriptions[1].offset = 0;

				attributeDescriptions[2].binding = 2;
				attributeDescriptions[2].location = 2;
				attributeDescriptions[2].format = vk::Format::eR32G32B32Sfloat;
				attributeDescriptions[2].offset = 0;

				attributeDescriptions[3].binding = 3;
				attributeDescriptions[3].location = 3;
				attributeDescriptions[3].format = vk::Format::eR32G32Sfloat;
				attributeDescriptions[3].offset = 0;

				attributeDescriptions[4].binding = 4;
				attributeDescriptions[4].location = 4;
				attributeDescriptions[4].format = vk::Format::eR32G32B32A32Sfloat;
				attributeDescriptions[4].offset = 0;

				attributeDescriptions[5].binding = 5;
				attributeDescriptions[5].location = 5;
				attributeDescriptions[5].format = vk::Format::eR32G32B32A32Sfloat;
				attributeDescriptions[5].offset = 0;

				attributeDescriptions[6].binding = 6;
				attributeDescriptions[6].location = 6;
				attributeDescriptions[6].format = vk::Format::eR32G32B32A32Sfloat;
				attributeDescriptions[6].offset = 0;

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
			engine::graphics::hashCombine(seed, vertex.pos, vertex.color, vertex.normal, vertex.texcoord, vertex.tangent, vertex.joint0, vertex.weight0);
			return seed;
		}
	};
}