#pragma once

#include <glm/glm.hpp>

namespace engine
{
	namespace graphics
	{
		class IDebugDrawInterface
		{
		public:
			virtual ~IDebugDrawInterface() = default;

			virtual void create() = 0;
			virtual void draw() = 0;

			virtual void drawDebugPoint(const glm::vec3& pos, const float size = 1.f, const glm::vec3& color = glm::vec3(1.f)) = 0;
			virtual void drawDebugLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color = glm::vec3(1.f)) = 0;
			virtual void drawDebugVector(const glm::vec3& origin, const glm::vec3& to, const float length, const glm::vec3& color = glm::vec3(1.f)) = 0;
			virtual void drawDebugTangentBasis(const glm::vec3& origin, const glm::vec3& normal, const glm::vec3& tangent, const glm::vec3& bitangent, const float lengths) = 0;
			virtual void drawDebugBox(const glm::vec3& center, const float width, const float height, const float depth, const glm::vec3& color = glm::vec3(1.f)) = 0;
			virtual void drawDebugBox(const std::array<glm::vec3, 8>& points, const glm::vec3& color = glm::vec3(1.f)) = 0;
			virtual void drawDebugAABB(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color = glm::vec3(1.f)) = 0;
			virtual void drawDebugArrow(const glm::vec3& from, const glm::vec3& to, const float size = 1.f, const glm::vec3& color = glm::vec3(1.f)) = 0;
			virtual void drawDebugCircle(const glm::vec3& center, const glm::vec3& planeNormal, const float radius, const int numSteps, const glm::vec3& color = glm::vec3(1.f)) = 0;
			virtual void drawDebugCone(const glm::vec3& apex, const glm::vec3& dir, const float radius, const float apexRadius, const glm::vec3& color = glm::vec3(1.f)) = 0;
			virtual void drawDebugCross(const glm::vec3& center, const float length) = 0;
			virtual void drawDebugFrustum(const glm::mat4& invClipMax, const glm::vec3& color = glm::vec3(1.f)) = 0;
			virtual void drawDebugPlane(const glm::vec3& center, const glm::vec3& normal, const float scale, const float normalScale, const glm::vec3& color = glm::vec3(1.f), const glm::vec3& normalColor = glm::vec3(0.f, 1.f, 0.f)) = 0;
			virtual void drawDebugSphere(const glm::vec3& center, const float radius, const glm::vec3& color = glm::vec3(1.f)) = 0;
			virtual void drawDebugGrid(const float mins, const float maxs, const float y, const float step, const glm::vec3& color = glm::vec3(0.3f)) = 0;
		};
	}
}