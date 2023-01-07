#pragma once

#include <utility/uparse.hpp>

namespace engine
{
	namespace ecs
	{
		struct FAudioComponent
		{
			std::string source;
			size_t asource;

			float gain{ 1.f };
			float pitch{ 1.f };
			glm::vec3 velocity;
			bool loop{ false };
			bool playing{ false };
			bool shouldStop{ false };
		};

		void to_json(nlohmann::json& json, const FAudioComponent& type);
		void from_json(const nlohmann::json& json, FAudioComponent& type);
	}
}