#pragma once

#include "CommandBase.h"
#include <glm/glm.hpp>

namespace engine
{
	namespace graphics
	{
		class CCommandTransform : public CCommandBase
		{
		public:
			void execute(vk::CommandBuffer& commandBuffer) override;
		private:
			glm::mat4* model{ nullptr };
			glm::mat4* model_old{ nullptr };
			glm::mat4* view{ nullptr };
			glm::mat4* projection{ nullptr };
			glm::mat4* normal{ nullptr };
			glm::vec3* view_dir{ nullptr };
			glm::vec2* viewport_dim{ nullptr };
			glm::vec4* frustum_planes[6]{ nullptr };
		};
	}
}