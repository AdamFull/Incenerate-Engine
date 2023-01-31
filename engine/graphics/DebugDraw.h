#pragma once

#include "APIStructures.h"

namespace engine
{
	namespace graphics
	{
		class CAPIHandle;

		class CDebugDraw
		{
		public:
			CDebugDraw(CAPIHandle* gapi);
			void create();
			void draw();

			void drawDebugPoint(const glm::vec3& pos, const float size = 1.f, const glm::vec3& color = glm::vec3(1.f));
			void drawDebugLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color = glm::vec3(1.f));
			void drawDebugVector(const glm::vec3& origin, const glm::vec3& to, const float length, const glm::vec3& color = glm::vec3(1.f));
			void drawDebugTangentBasis(const glm::vec3& origin, const glm::vec3& normal, const glm::vec3& tangent, const glm::vec3& bitangent, const float lengths);
			void drawDebugBox(const glm::vec3& center, const float width, const float height, const float depth, const glm::vec3& color = glm::vec3(1.f));
			void drawDebugBox(const std::array<glm::vec3, 8>& points, const glm::vec3& color = glm::vec3(1.f));
			void drawDebugAABB(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color = glm::vec3(1.f));
			void drawDebugArrow(const glm::vec3& from, const glm::vec3& to, const float size = 1.f, const glm::vec3& color = glm::vec3(1.f));
			void drawDebugCircle(const glm::vec3& center, const glm::vec3& planeNormal, const float radius, const int numSteps, const glm::vec3& color = glm::vec3(1.f));
			void drawDebugCone(const glm::vec3& apex, const glm::vec3& dir, const float radius, const float apexRadius, const glm::vec3& color = glm::vec3(1.f));
			void drawDebugCross(const glm::vec3& center, const float length);
			void drawDebugFrustum(const glm::mat4& invClipMax, const glm::vec3& color = glm::vec3(1.f));
			void drawDebugPlane(const glm::vec3& center, const glm::vec3& normal, const float scale, const float normalScale, const glm::vec3& color = glm::vec3(1.f), const glm::vec3& normalColor = glm::vec3(0.f, 1.f, 0.f));
			void drawDebugSphere(const glm::vec3& center, const float radius, const glm::vec3& color = glm::vec3(1.f));
			void drawDebugGrid(const float mins, const float maxs, const float y, const float step, const glm::vec3& color = glm::vec3(0.3f));
			
		private:
			CAPIHandle* graphics{ nullptr };
			std::vector<FVertex> vDrawData;

			size_t vbo_id{ invalid_index };
			size_t shader_id{ invalid_index };
		};
	}
}