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

                glm::vec3 deltaPos1 = v1.pos - v0.pos;
                glm::vec3 deltaPos2 = v2.pos - v0.pos;

                auto normal = glm::normalize(glm::cross(deltaPos1, deltaPos2));
                v0.normal += normal;
                v1.normal += normal;
                v2.normal += normal;
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