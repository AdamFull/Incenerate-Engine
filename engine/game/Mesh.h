#pragma once
#include "GameObject.h"
#include "EngineStructures.h"

namespace engine
{
	namespace game
	{
		struct FMeshlet
		{
			size_t begin_index, index_count;
			size_t begin_vertex, vertex_count;
			size_t vertex_buffer{ invalid_index };
			size_t material{ invalid_index };

			bool bWasCulled{ false };
			bool bIsSkybox{ false };

			struct Dimensions
			{
				glm::vec3 min = glm::vec3(FLT_MAX);
				glm::vec3 max = glm::vec3(-FLT_MAX);
				glm::vec3 size;
				glm::vec3 center;
				float radius;
			} dimensions;

			void setDimensions(glm::vec3 min, glm::vec3 max);
		};

		class CMesh : public CGameObject
		{
		public:
			virtual ~CMesh() = default;

			void create() override;
			void reCreate() override;
			void render() override;
			void update(float fDeltaTime) override;

			void addMeshlet(FMeshlet&& meshlet);

		private:
			std::vector<glm::vec4> vInstances;
			std::vector<FMeshlet> vMeshlets;

			bool bEnableCulling{ true };
			bool bHasInstances{ false };
			ECullingType eCullingType{ ECullingType::eByBox };
			ERenderStageAvaliableFlags stageFlags{ (uint32_t)ERenderStageAvaliableFlagBits::eShadow | (uint32_t)ERenderStageAvaliableFlagBits::eMesh };
		};
	}
}