#pragma once

#include "CommandBase.h"

namespace engine
{
	namespace graphics
	{
		class CCommandDrawMeshlet
		{
		public:
			virtual void execute(vk::CommandBuffer& commandBuffer) = 0;
		};
	}
}