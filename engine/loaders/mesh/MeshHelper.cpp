#include "MeshHelper.h"

namespace engine
{
	namespace loaders
	{
        void calculate_normals(std::vector<graphics::FVertex>& vertices, const std::vector<uint32_t>& indices, uint64_t startVertex)
        {
            for (auto idx = 0; idx < indices.size(); idx += 3)
            {
                auto& v0 = vertices.at(indices.at(idx) - startVertex);
                auto& v1 = vertices.at(indices.at(idx + 1) - startVertex);
                auto& v2 = vertices.at(indices.at(idx + 2) - startVertex);

                v0.normal += glm::normalize(glm::cross(v1.pos - v0.pos, v2.pos - v0.pos));
                v1.normal += glm::normalize(glm::cross(v2.pos - v1.pos, v0.pos - v1.pos));
                v2.normal += glm::normalize(glm::cross(v0.pos - v2.pos, v1.pos - v2.pos));
            }

            for (auto& vertex : vertices)
                vertex.normal = glm::normalize(vertex.normal);
        }

        void calculate_normals_quads(std::vector<graphics::FVertex>& vertices, const std::vector<uint32_t>& indices, uint64_t startVertex)
        {
            for (auto idx = 0; idx < indices.size(); idx += 4)
            {
                auto& v0 = vertices.at(indices.at(idx) - startVertex);
                auto& v1 = vertices.at(indices.at(idx + 1) - startVertex);
                auto& v2 = vertices.at(indices.at(idx + 2) - startVertex);
                auto& v3 = vertices.at(indices.at(idx + 3) - startVertex);

                auto n0 = glm::normalize(glm::cross(v1.pos - v0.pos, v3.pos - v0.pos));
                auto n1 = glm::normalize(glm::cross(v2.pos - v1.pos, v0.pos - v1.pos));
                auto n2 = glm::normalize(glm::cross(v3.pos - v2.pos, v1.pos - v2.pos));
                auto n3 = glm::normalize(glm::cross(v0.pos - v3.pos, v2.pos - v3.pos));

                v0.normal += n0 + n3;
                v1.normal += n0 + n1;
                v2.normal += n1 + n2;
                v3.normal += n2 + n3;
            }

            for (auto& vertex : vertices)
                vertex.normal = glm::normalize(vertex.normal);
        }

        void calculate_tangents(std::vector<graphics::FVertex>& vertices, const std::vector<uint32_t>& indices, uint64_t startVertex)
        {
            for (auto idx = 0; idx < indices.size(); idx += 3)
            {
                auto& v0 = vertices.at(indices.at(idx) - startVertex);
                auto& v1 = vertices.at(indices.at(idx + 1) - startVertex);
                auto& v2 = vertices.at(indices.at(idx + 2) - startVertex);

                glm::vec3 deltaPos1 = v1.pos - v0.pos;
                glm::vec3 deltaPos2 = v2.pos - v0.pos;

                glm::vec2 deltaUV1 = v1.texcoord - v0.texcoord;
                glm::vec2 deltaUV2 = v2.texcoord - v0.texcoord;

                float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
                glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
                tangent = glm::normalize(tangent);

                float w = (glm::dot(glm::cross(v0.normal, tangent), tangent) < 0.0f) ? -1.0f : 1.0f;
                v0.tangent = v1.tangent = v2.tangent = glm::vec4(tangent, w);
            }

            for (auto& vertex : vertices)
                vertex.tangent = glm::normalize(vertex.tangent);
        }
	}
}