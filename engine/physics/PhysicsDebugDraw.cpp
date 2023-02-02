#include "PhysicsDebugDraw.h"

#include "Engine.h"

#include "PhysicsHelper.h"

using namespace engine::graphics;
using namespace engine::physics;

void CPhysicsDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	auto& graphics = EGEngine->getGraphics();
	auto& debug_draw = graphics->getDebugDraw();
	debug_draw->drawDebugLine(btVector3_to_vec3(from), btVector3_to_vec3(to), btVector3_to_vec3(color));
}

void CPhysicsDebugDraw::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	drawLine(PointOnB, PointOnB + normalOnB * distance, color);
}

void CPhysicsDebugDraw::reportErrorWarning(const char* warningString)
{
	log_warning("BULLET: {}", warningString);
}

void CPhysicsDebugDraw::draw3dText(const btVector3& location, const char* textString)
{

}

void CPhysicsDebugDraw::setDebugMode(int debugMode)
{
	this->debugMode = debugMode;
}

int CPhysicsDebugDraw::getDebugMode() const
{
	return debugMode;
}