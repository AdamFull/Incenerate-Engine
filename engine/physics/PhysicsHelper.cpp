#include "PhysicsHelper.h"

namespace engine
{
	namespace physics
	{
		glm::vec3 btVector3_to_vec3(const btVector3& vec)
		{
			return glm::vec3(vec.x(), vec.y(), vec.z());
		}

		btVector3 vec3_to_btVector3(const glm::vec3& vec)
		{
			return btVector3(vec.x, vec.y, vec.z);
		}

		glm::mat4 btTransform_to_mat4(const btTransform& bttransform)
		{
			
			glm::mat4 mat;
			bttransform.getOpenGLMatrix(glm::value_ptr(mat));
			return mat;
		}

		btTransform mat4_to_btTransform(const glm::mat4& mat)
		{
			btTransform transform;
			transform.setFromOpenGLMatrix(glm::value_ptr(mat));
			return transform;
		}
	}
}