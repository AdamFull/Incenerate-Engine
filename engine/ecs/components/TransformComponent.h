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
			glm::vec3 rotation{ 0.f };
			glm::vec3 scale{ 1.f };
			glm::vec3 rposition{ 0.f };
			glm::vec3 rrotation{ 0.f };
			glm::vec3 rscale{ 1.f };
			glm::mat4 model{ 1.f };
			glm::mat4 model_old{ 1.f };
			glm::mat4 normal{ 1.f };

			void update()
			{
				glm::vec3 skew;
				glm::quat qrotation;
				glm::vec4 perspective;
				glm::decompose(model, rscale, qrotation, rposition, skew, perspective);
				qrotation = glm::conjugate(qrotation);
				rrotation = glm::eulerAngles(qrotation);
			}

			void apply_delta(const glm::mat4& mat)
			{
				glm::vec3 dposition;
				glm::vec3 drotation;
				glm::vec3 dscale;
				decompose(mat, dposition, drotation, dscale);
				position += dposition;
				rotation += drotation;
			}
		};

		void to_json(nlohmann::json& json, const FTransformComponent& type);
		void from_json(const nlohmann::json& json, FTransformComponent& type);
	}
}