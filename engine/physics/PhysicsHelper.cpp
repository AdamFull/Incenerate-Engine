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
			auto rot = bttransform.getBasis();
			auto row = rot.getRow(0);
			mat[0].x = row.getX();
			mat[1].x = row.getY();
			mat[2].x = row.getZ();
			row = rot.getRow(1);
			mat[0].y = row.getX();
			mat[1].y = row.getY();
			mat[2].y = row.getZ();
			row = rot.getRow(2);
			mat[0].z = row.getX();
			mat[1].z = row.getY();
			mat[2].z = row.getZ();

			row = bttransform.getOrigin();
			mat[0].w = row.getX();
			mat[1].w = row.getY();
			mat[2].w = row.getZ();

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