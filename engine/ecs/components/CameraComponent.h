#pragma once

#include "graphics/Frustum.h"

namespace engine
{
	namespace ecs
	{
		struct FCameraComponent
		{
			FFrustum frustum;

			float aspect{ 1.7f };
			float fieldOfView{ 45.f };
			float nearPlane{ 0.1f };
			float farPlane{ 128.f }; 
			float sensitivity{ 15.f };

			float angleH{ 0.f };
			float angleV{ 0.f };

			glm::mat4 view, invView, projection, invProjection;
			glm::vec2 viewportDim{};

			bool bMoved{ true };
			bool bActive{ true };
		};
	}
}