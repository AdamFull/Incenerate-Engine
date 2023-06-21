#pragma once

#include "Helpers.h"

namespace engine
{
	namespace ecs
	{
		struct FTransformComponent
		{
			FTransformComponent() = default;

			glm::vec3 position{ 0.f };
			glm::quat rotation{ 1.f, 0.f, 0.f, 0.f };
			glm::vec3 scale{ 1.f };
			glm::vec3 rposition{ 0.f };
			glm::quat rrotation{ 1.f, 0.f, 0.f, 0.f };
			glm::vec3 rscale{ 1.f };
			glm::mat4 matrix{ 1.f };
			glm::mat4 model{ 1.f };
			glm::mat4 model_old{ 1.f };
			glm::mat4 normal{ 1.f };

			void update()
			{
				glm::vec3 skew;
				glm::vec4 perspective;
				glm::decompose(model, rscale, rrotation, rposition, skew, perspective);
				//rrotation = glm::conjugate(rrotation);
			}

			void apply_delta(const glm::mat4& mat)
			{
				glm::vec3 skew;
				glm::vec4 perspective;
				glm::vec3 dposition;
				glm::quat drotation;
				glm::vec3 dscale;
				glm::decompose(model, dscale, drotation, dposition, skew, perspective);
				position += dposition;
				rotation += drotation;
			}
		};

		void to_json(nlohmann::json& json, const FTransformComponent& type);
		void from_json(const nlohmann::json& json, FTransformComponent& type);
	}
}