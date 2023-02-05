#pragma once

namespace engine
{
	namespace ecs
	{
		struct FParticleComponent
		{
			std::string source;
			size_t particle_id{ invalid_index };
			bool loaded{ false };
		};

		void to_json(nlohmann::json& json, const FParticleComponent& type);
		void from_json(const nlohmann::json& json, FParticleComponent& type);
	}
}