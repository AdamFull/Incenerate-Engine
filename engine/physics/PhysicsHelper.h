#pragma once

#include <bullet3/src/LinearMath/btTransform.h>
#include <bullet3/src/LinearMath/btVector3.h>
#include <bullet3/src/LinearMath/btQuaternion.h>

namespace engine
{
	namespace physics
	{
		glm::vec3 btVector3_to_vec3(const btVector3& btvec);
		btVector3 vec3_to_btVector3(const glm::vec3& vec);
		glm::mat4 btTransform_to_mat4(const btTransform& bttransform);
		btTransform mat4_to_btTransform(const glm::mat4& mat);
	}
}