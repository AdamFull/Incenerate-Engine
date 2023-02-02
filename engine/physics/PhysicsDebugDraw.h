#pragma once

#include <bullet3/src/LinearMath/btIDebugDraw.h>

namespace engine
{
	namespace physics
	{
		class CPhysicsDebugDraw : public btIDebugDraw
		{
		public:
			CPhysicsDebugDraw(graphics::CAPIHandle* handle);

			virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
			virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
			virtual void reportErrorWarning(const char* warningString) override;
			virtual void draw3dText(const btVector3& location, const char* textString) override;
			virtual void setDebugMode(int debugMode) override;
			virtual int getDebugMode() const override;
		private:
			graphics::CAPIHandle* graphics{ nullptr };
			int32_t debugMode { DBG_DrawWireframe | DBG_DrawConstraints | DBG_DrawConstraintLimits };
		};
	}
}