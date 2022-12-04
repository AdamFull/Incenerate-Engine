#pragma once

namespace engine
{
	namespace graphics
	{
		class CCommandBase
		{
		public:
			virtual void execute(vk::CommandBuffer& commandBuffer) = 0;
		};
	}
}