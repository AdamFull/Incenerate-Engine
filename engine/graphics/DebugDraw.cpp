#include "Engine.h"

#include "ecs/components/CameraComponent.h"

using namespace engine::graphics;

#define DEBUG_DRAW_MAX_VERTICES 4096

CDebugDraw::CDebugDraw(CAPIHandle* gapi)
{
	graphics = gapi;
}

void CDebugDraw::create()
{
	vbo_id = graphics->addVertexBuffer("debug_draw_vbo");

	auto& vbo = graphics->getVertexBuffer(vbo_id);
	vbo->create(DEBUG_DRAW_MAX_VERTICES, 0);

	shader_id = graphics->addShader("debug_draw_shader", "debugdraw");
}

void CDebugDraw::draw()
{
	auto* camera = EGEngine->getActiveCamera();

	if (!camera)
		return;

	if (!vDrawData.empty())
	{
		auto& vbo = graphics->getVertexBuffer(vbo_id);
		vbo->update(vDrawData);

		graphics->bindShader(shader_id);
		graphics->bindVertexBuffer(vbo_id);

		auto& pUniform = graphics->getUniformHandle("UBODebugDraw");
		pUniform->set("projection", camera->projection);
		pUniform->set("view", camera->view);

		graphics->draw(0, vDrawData.size(), 0, 0, 1);

		graphics->bindVertexBuffer(invalid_index);
		graphics->bindShader(invalid_index);


		vDrawData.clear();
	}
}

void CDebugDraw::drawDebugAABB(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color, int duration)
{

}

void CDebugDraw::drawDebugArror(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color, int duration)
{

}

void CDebugDraw::drawDebugAxis(const glm::mat4& transform, const float size, const float length, int duration)
{

}

void CDebugDraw::drawDebugBox(const std::array<glm::vec3, 8>& points, const glm::vec3& color, int duration)
{

}

void CDebugDraw::drawDebugCircle(const glm::vec3& center, const glm::vec3& planeNormal, const float radius, const float numSteps, const glm::vec3& color, int duration)
{

}

void CDebugDraw::drawDebugCone(const glm::vec3& apex, const glm::vec3& dir, const float radius, const float apexRadius, const glm::vec3& color, int duration)
{

}

void CDebugDraw::drawDebugCross(const glm::vec3& center, const float length, int duration)
{

}

void CDebugDraw::drawDebugFrustum(const glm::mat4& invClipMax, const glm::vec3& color, int duration)
{

}

void CDebugDraw::drawDebugLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color, int duration)
{
	vDrawData.emplace_back(FVertex{ from, color });
	vDrawData.emplace_back(FVertex{ to, color });
}

void CDebugDraw::drawDebugPlane(const glm::vec3& center, const glm::vec3& normal, const float scale, const float normalScale, const glm::vec3& color, const glm::vec3& normalColor, int duration)
{

}

void CDebugDraw::drawDebugPoint(const glm::vec3& pos, const float size, const glm::vec3& color, int duration)
{

}