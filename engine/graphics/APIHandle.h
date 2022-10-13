#pragma once

#include "Device.h"
#include "EngineEnums.h"

namespace engine
{
	namespace graphics
	{
		class CAPIHandle
		{
		public:
			void create(const FEngineCreateInfo& createInfo);
			void begin();
			void end();

			ERenderApi getAPI() { return eAPI; }

		protected:
			ERenderApi eAPI;
			std::unique_ptr<CDevice> pDevice;
		};
	}
}