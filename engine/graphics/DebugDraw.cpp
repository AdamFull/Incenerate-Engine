// Based on: https://github.com/glampert/debug-draw/blob/master/debug_draw.hpp
#include "Engine.h"

#include "ecs/components/CameraComponent.h"

using namespace engine::graphics;

#define DEBUG_DRAW_MAX_VERTICES 16384

CDebugDraw::CDebugDraw(CAPIHandle* gapi)
{
	graphics = gapi;
}

#if !defined(INCENERATE_RELEASE_APP)

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

void CDebugDraw::drawDebugPoint(const glm::vec3& pos, const float size, const glm::vec3& color)
{

}

void CDebugDraw::drawDebugLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color)
{
	vDrawData.emplace_back(FVertex{ from, color });
	vDrawData.emplace_back(FVertex{ to, color });
}

void CDebugDraw::drawDebugVector(const glm::vec3& origin, const glm::vec3& to, const float length, const glm::vec3& color)
{
	auto vec = (to * length) + origin;
	drawDebugLine(origin, vec);
}

void CDebugDraw::drawDebugTangentBasis(const glm::vec3& origin, const glm::vec3& normal, const glm::vec3& tangent, const glm::vec3& bitangent, const float lengths)
{
	drawDebugVector(origin, normal, lengths);
	drawDebugVector(origin, tangent, lengths, glm::vec3(1.f, 1.f, 0.f));
	drawDebugVector(origin, bitangent, lengths, glm::vec3(1.f, 0.f, 1.f));
}

void CDebugDraw::drawDebugBox(const glm::vec3& center, const float width, const float height, const float depth, const glm::vec3& color)
{
	std::array<glm::vec3, 8> points;

	const float w = width * 0.5f;
	const float h = height * 0.5f;
	const float d = depth * 0.5f;

	points[0] = glm::vec3(center.x - w, center.y + h, center.z + d);
	points[1] = glm::vec3(center.x - w, center.y + h, center.z - d);
	points[2] = glm::vec3(center.x + w, center.y + h, center.z - d);
	points[3] = glm::vec3(center.x + w, center.y + h, center.z + d);
	points[4] = glm::vec3(center.x - w, center.y - h, center.z + d);
	points[5] = glm::vec3(center.x - w, center.y - h, center.z - d);
	points[6] = glm::vec3(center.x + w, center.y - h, center.z - d);
	points[7] = glm::vec3(center.x + w, center.y - h, center.z + d);

	drawDebugBox(points, color);
}

void CDebugDraw::drawDebugBox(const std::array<glm::vec3, 8>& points, const glm::vec3& color)
{
	for (uint32_t i = 0; i < 4; ++i)
	{
		drawDebugLine(points[i], points[(i + 1) & 3], color);
		drawDebugLine(points[4 + i], points[4 + ((i + 1) & 3)], color);
		drawDebugLine(points[i], points[4 + i], color);
	}
}

void CDebugDraw::drawDebugAABB(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color)
{
	const std::array<glm::vec3, 2> bb{ start, end };
	std::array<glm::vec3, 8> points;

	for (uint32_t i = 0; i < 8; ++i)
	{
		points[i].x = bb[(i ^ (i >> 1)) & 1].x;
		points[i].y = bb[(i >> 1) & 1].y;
		points[i].z = bb[(i >> 2) & 1].z;
	}

	drawDebugBox(points, color);
}

void CDebugDraw::drawDebugArrow(const glm::vec3& from, const glm::vec3& to, const float size, const glm::vec3& color)
{
	static const float arrowStep = 30.0f; // In degrees
	static const float arrowSin[45] = {
		0.0f, 0.5f, 0.866025f, 1.0f, 0.866025f, 0.5f, -0.0f, -0.5f, -0.866025f,
		-1.0f, -0.866025f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
	};
	static const float arrowCos[45] = {
		1.0f, 0.866025f, 0.5f, -0.0f, -0.5f, -0.866026f, -1.0f, -0.866025f, -0.5f, 0.0f,
		0.5f, 0.866026f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
	};

	drawDebugLine(from, to, color);

	glm::vec3 up, right, forward;
	forward = glm::normalize(to - from);
	right = glm::normalize(glm::cross(forward, glm::vec3{ 0.0, 1.0, 0.0 }));
	up = glm::normalize(glm::cross(right, forward));
	forward *= size;

	float degrees{ 0.f };
	for (int i = 0; degrees < 360.0f; degrees += arrowStep, ++i)
	{
		float scale;
		glm::vec3 v1, v2, temp;

		scale = 0.5f * size * arrowCos[i];
		temp = right * scale;
		v1 = to - forward;
		v1 = v1 + temp;

		scale = 0.5f * size * arrowSin[i];
		temp = up * scale;
		v1 = v1 + temp;

		scale = 0.5f * size * arrowCos[i + 1];
		temp = right * scale;
		v2 = to - forward;
		v2 = v2 + temp;

		scale = 0.5f * size * arrowSin[i + 1];
		temp = up * scale;
		v2 = v2 + temp;

		drawDebugLine(v1, to, color);
		drawDebugLine(v1, v2, color);
	}
}

void CDebugDraw::drawDebugCircle(const glm::vec3& center, const glm::vec3& planeNormal, const float radius, const int numSteps, const glm::vec3& color)
{
	glm::vec3 left, up;
	glm::vec3 point, lastPoint;

	left = glm::normalize(glm::cross(planeNormal, glm::vec3{ 0.0, 1.0, 0.0 }));
	up = glm::normalize(glm::cross(left, planeNormal));

	up *= radius;
	left *= radius;
	lastPoint = center + up;

	for (int i = 1; i <= numSteps; ++i)
	{
		const float radians = glm::two_pi<float>() * i / numSteps;

		glm::vec3 vs, vc;
		vs = left * glm::sin(radians);
		vc = up * glm::cos(radians);

		point = center + vs;
		point = point + vc;

		drawDebugLine(lastPoint, point, color);
		lastPoint = point;
	}
}

void CDebugDraw::drawDebugCone(const glm::vec3& apex, const glm::vec3& dir, const float radius, const float apexRadius, const glm::vec3& color)
{
	static const int stepSize = 20;
	std::array<glm::vec3, 3> axis;
	glm::vec3 top, temp0, temp1, temp2;
	glm::vec3 p1, p2, lastP1, lastP2;

	axis[2] = glm::normalize(dir);
	axis[0] = glm::normalize(glm::cross(axis[2], glm::vec3{ 0.0, 1.0, 0.0 }));
	axis[1] = glm::normalize(glm::cross(axis[0], axis[2]));
	axis[1] *= -1.f;

	top = apex + dir;
	temp1 = axis[1] * radius;
	lastP2 = top + temp1;

	if (apexRadius == 0.0f)
	{
		for (int i = stepSize; i <= 360; i += stepSize)
		{
			temp1 = axis[0] * glm::sin(glm::radians(static_cast<float>(i)));
			temp2 = axis[1] * glm::cos(glm::radians(static_cast<float>(i)));
			temp0 = temp1 + temp2;

			temp0 *= radius;
			p2 = top + temp0;

			drawDebugLine(lastP2, p2, color);
			drawDebugLine(p2, apex, color);

			lastP2 = p2;
		}
	}
	else // A degenerate cone with open apex:
	{
		temp1 = axis[1] * apexRadius;
		lastP1 = apex + temp1;

		for (int i = stepSize; i <= 360; i += stepSize)
		{
			temp1 = axis[0] * glm::sin(glm::radians(static_cast<float>(i)));
			temp2 = axis[1] * glm::cos(glm::radians(static_cast<float>(i)));
			temp0 = temp1 + temp2;

			temp1 = temp0 * apexRadius;
			temp2 = temp0 * radius;

			p1 = apex + temp1;
			p2 = top + temp2;

			drawDebugLine(lastP1, p1, color);
			drawDebugLine(lastP2, p2, color);
			drawDebugLine(p1, p2, color);

			lastP1 = p1;
			lastP2 = p2;
		}
	}
}

void CDebugDraw::drawDebugCross(const glm::vec3& center, const float length)
{
	glm::vec3 from, to;
	glm::vec3 cR{ 1.0f, 0.0f, 0.0f }, cG{ 0.0f, 1.0f, 0.0f }, cB{ 0.0f, 0.0f, 1.0f };

	const float hl = length * 0.5f; // Half on each side.

	// Red line: X - length/2 to X + length/2
	from = { center.x - hl, center.y, center.z };
	to = { center.x + hl, center.y, center.z };
	drawDebugLine(from, to, cR);

	// Green line: Y - length/2 to Y + length/2
	from = { center.x, center.y - hl, center.z };
	to = { center.x, center.y + hl, center.z };
	drawDebugLine(from, to, cG);

	// Blue line: Z - length/2 to Z + length/2
	from = { center.x, center.y, center.z - hl };
	to = { center.x, center.y, center.z + hl };
	drawDebugLine(from, to, cB);
}

void CDebugDraw::drawDebugFrustum(const glm::mat4& invClipMax, const glm::vec3& color)
{
	static const std::array<glm::vec3, 8> planes = {
		// near plane
		glm::vec3{ -1.0f, -1.0f, -1.0f }, glm::vec3{  1.0f, -1.0f, -1.0f },
		glm::vec3{  1.0f,  1.0f, -1.0f }, glm::vec3{ -1.0f,  1.0f, -1.0f },
		// far plane
		glm::vec3{ -1.0f, -1.0f,  1.0f }, glm::vec3{  1.0f, -1.0f,  1.0f },
		glm::vec3{  1.0f,  1.0f,  1.0f }, glm::vec3{ -1.0f,  1.0f,  1.0f }
	};

	std::array<glm::vec3, 8> points;
	for (uint32_t i = 0; i < planes.size(); ++i)
	{
		auto v = invClipMax * glm::vec4(planes[i], 1.0f);
		points[i] = glm::vec3(v) / v.w;
	}

	// Connect the dots:
	drawDebugBox(points, color);
}

void CDebugDraw::drawDebugPlane(const glm::vec3& center, const glm::vec3& normal, const float scale, const float normalScale, const glm::vec3& color, const glm::vec3& normalColor)
{
	glm::vec3 v1, v2, v3, v4;
	glm::vec3 tangent, bitangent;

	tangent = glm::normalize(glm::cross(normal, glm::vec3{ 0.0, 1.0, 0.0 }));
	bitangent = glm::normalize(glm::cross(tangent, normal));

	v1 = center - (tangent * scale) - (bitangent * scale);
	v2 = center + (tangent * scale) - (bitangent * scale);
	v3 = center + (tangent * scale) + (bitangent * scale);
	v4 = center - (tangent * scale) + (bitangent * scale);

	// Draw the wireframe plane quadrilateral:
	drawDebugLine(v1, v2, color);
	drawDebugLine(v2, v3, color);
	drawDebugLine(v3, v4, color);
	drawDebugLine(v4, v1, color);

	// Optionally add a line depicting the plane normal:
	if (normalScale != 0.0f)
	{
		glm::vec3 normalVec = (normal * normalScale) + center;
		drawDebugLine(center, normalVec, normalColor);
	}
}

void CDebugDraw::drawDebugSphere(const glm::vec3& center, const float radius, const glm::vec3& color)
{
	static const int stepSize = 15;
	std::array<glm::vec3, 360 / stepSize> cache;
	glm::vec3 radiusVec = glm::vec3(0.f, 0.f, radius);

	cache[0] = center + radiusVec;

	for (int n = 1; n < cache.size(); ++n)
		cache[n] = cache[0];

	glm::vec3 lastPoint, temp;
	for (int i = stepSize; i <= 360; i += stepSize)
	{
		const float s = glm::sin(glm::radians(static_cast<float>(i)));
		const float c = glm::cos(glm::radians(static_cast<float>(i)));

		lastPoint = { center.x, center.y + radius * s, center.z + radius * c };

		for (int n = 0, j = stepSize; j <= 360; j += stepSize, ++n)
		{
			temp = { center.x + glm::sin(glm::radians(static_cast<float>(j))) * radius * s, center.y + glm::cos(glm::radians(static_cast<float>(j))) * radius * s, lastPoint.z };

			drawDebugLine(lastPoint, temp, color);
			drawDebugLine(lastPoint, cache[n], color);

			cache[n] = lastPoint;
			lastPoint = temp;
		}
	}
}

void CDebugDraw::drawDebugGrid(const float mins, const float maxs, const float y, const float step, const glm::vec3& color)
{
	glm::vec3 from, to;
	for (float i = mins; i <= maxs; i += step)
	{
		// Horizontal line (along the X)
		from = glm::vec3(mins, y, i);
		to = glm::vec3(maxs, y, i);
		drawDebugLine(from, to, color);

		// Vertical line (along the Z)
		from = glm::vec3(i, y, mins);
		to = glm::vec3(i, y, maxs);
		drawDebugLine(from, to, color);
	}
}

#endif