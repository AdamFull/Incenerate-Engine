#pragma once

#include <utility/uparse.hpp>

namespace engine
{
	namespace ecs
	{
		struct FAudioComponent
		{
			std::string source;
			float gain{ 1.f };
			float pitch{ 1.f };
			glm::vec3 velocity;
			bool loop;
			bool playing;
			bool shouldStop;
		};

		void to_json(nlohmann::json& json, const FAudioComponent& type);
		void from_json(const nlohmann::json& json, FAudioComponent& type);
	}
}