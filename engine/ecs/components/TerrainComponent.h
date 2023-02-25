#pragma once

namespace engine
{
	namespace ecs
	{
		struct FTerrainComponent
		{
			std::string source{};
			uint32_t size{ 256u };
			float uv_scale{ 1.f };
			bool useTesselation{ true };

			glm::vec3 min{};
			glm::vec3 max{};

			size_t heightmap_id{ invalid_index };
			size_t vbo_id{ invalid_index };
			size_t material_id{ invalid_index };

			bool loaded{ false };
		};

		void to_json(nlohmann::json& json, const FTerrainComponent& type);
		void from_json(const nlohmann::json& json, FTerrainComponent& type);
	}
}