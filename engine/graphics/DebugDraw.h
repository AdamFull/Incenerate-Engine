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

			void drawDebugAABB(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color = glm::vec3(1.f), int duration = 0);
			void drawDebugArror(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color = glm::vec3(1.f), int duration = 0);
			void drawDebugAxis(const glm::mat4& transform, const float size, const float length, int duration = 0);
			void drawDebugBox(const std::array<glm::vec3, 8>& points, const glm::vec3& color = glm::vec3(1.f), int duration = 0);
			void drawDebugCircle(const glm::vec3& center, const glm::vec3& planeNormal, const float radius, const float numSteps, const glm::vec3& color = glm::vec3(1.f), int duration = 0);
			void drawDebugCone(const glm::vec3& apex, const glm::vec3& dir, const float radius, const float apexRadius, const glm::vec3& color = glm::vec3(1.f), int duration = 0);
			void drawDebugCross(const glm::vec3& center, const float length, int duration = 0);
			void drawDebugFrustum(const glm::mat4& invClipMax, const glm::vec3& color = glm::vec3(1.f), int duration = 0);
			void drawDebugLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color = glm::vec3(1.f), int duration = 0);
			void drawDebugPlane(const glm::vec3& center, const glm::vec3& normal, const float scale, const float normalScale, const glm::vec3& color = glm::vec3(1.f), const glm::vec3& normalColor = glm::vec3(1.f), int duration = 0);
			void drawDebugPoint(const glm::vec3& pos, const float size = 1.f, const glm::vec3& color = glm::vec3(1.f), int duration = 0);
		private:
			CAPIHandle* graphics{ nullptr };
			std::vector<FVertex> vDrawData;

			size_t vbo_id{ invalid_index };
			size_t shader_id{ invalid_index };
		};
	}
}