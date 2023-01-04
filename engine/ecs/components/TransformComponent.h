#pragma once

#include <utility/uparse.hpp>

namespace engine
{
	namespace ecs
	{
		struct FTransformComponent
		{
			FTransformComponent() = default;

			glm::vec3 position{ 0.f };
			glm::vec3 rotation{ 0.f };
			glm::vec3 scale{ 1.f, 1.f, 1.f };

			FTransformComponent& operator+=(const FTransformComponent& rhs)
			{
				this->scale *= rhs.scale;
				this->position = this->position * rhs.scale + rhs.position;
				this->rotation += rhs.rotation;
				return *this;
			}

			inline const glm::vec3& getPosition() const { return position; }
			inline void setPosition(const glm::vec3& position) { this->position = position; }

			inline const glm::vec3& getRotation() const { return rotation; }
			inline void setRotation(const glm::vec3& rotation) { this->rotation = rotation; }

			inline const glm::vec3& getScale() const { return scale; }
			inline void setScale(const glm::vec3& _scale) { this->scale = scale; }

			const glm::mat4 getModel()
			{
				glm::mat4 model{ 1.0 };

				model = glm::translate(model, position);

				if (rotation.x != 0) model = glm::rotate(model, rotation.x, glm::vec3(1.0, 0.0, 0.0));
				if (rotation.y != 0) model = glm::rotate(model, rotation.y, glm::vec3(0.0, 1.0, 0.0));
				if (rotation.z != 0) model = glm::rotate(model, rotation.z, glm::vec3(0.0, 0.0, 1.0));

				model = glm::scale(model, scale);

				return model;
			}
		};

		void to_json(nlohmann::json& json, const FTransformComponent& type)
		{
			//json = nlohmann::json
			//{
			//	{"pos", type.position},
			//	{"rot", type.rotation},
			//	{"scale", type.scale}
			//};
		}

		void from_json(const nlohmann::json& json, FTransformComponent& type)
		{
			//utl::parse_to("pos", json, type.position);
			//utl::parse_to("rot", json, type.rotation);
			//utl::parse_to("scale", json, type.scale);
		}
	}
}