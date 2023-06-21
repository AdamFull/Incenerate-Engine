#pragma once

#include "graphics/Frustum.h"
#include "EngineStructures.h"

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
			FPostProcess effects;

			float fieldOfView{ 45.f };
			float nearPlane{ 0.1f };
			float farPlane{ 4096.f };
			float sensitivity{ 15.f };

			float xmag{ 0.f };
			float ymag{ 0.f };

			glm::vec3 viewPos{ 0.f };
			glm::mat4 view{ 1.f }, invView{ 1.f }, projection{ 1.f }, invProjection{ 1.f };
			glm::vec2 viewportDim{ 0.f };

			glm::vec3 up{ 0.f };
			glm::vec3 forward{ 0.f };
			glm::vec3 right{ 0.f };

			bool moved{ true };
			bool active{ true };
			bool controllable{ true };
		};

		void to_json(nlohmann::json& json, const FCameraComponent& type);
		void from_json(const nlohmann::json& json, FCameraComponent& type);
	}
}