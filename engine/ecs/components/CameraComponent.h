#pragma once

#include "graphics/Frustum.h"
#include <utility/uparse.hpp>

namespace engine
{
	namespace ecs
	{
		enum class ECameraType
		{
			eOrthographic,
			ePerspective
		};

		struct FCameraComponent
		{
			ECameraType type{ ECameraType::ePerspective };

			FFrustum frustum;

			float aspect{ 1.7f };
			float fieldOfView{ 45.f };
			float nearPlane{ 0.1f };
			float farPlane{ 128.f }; 
			float sensitivity{ 15.f };

			float xmag{ 0.f };
			float ymag{ 0.f };

			float angleH{ 0.f };
			float angleV{ 0.f };

			glm::mat4 view, invView, projection, invProjection;
			glm::vec2 viewportDim{};
			glm::vec3 up;
			glm::vec3 forward;
			glm::vec3 right;

			bool moved{ true };
			bool active{ false };
		};

		void to_json(nlohmann::json& json, const FCameraComponent& type);
		void from_json(const nlohmann::json& json, FCameraComponent& type);
	}
}