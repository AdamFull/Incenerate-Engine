#pragma once

#include "oalhelp.h"

namespace engine
{
	namespace audio
	{
		class CAudioSystem
		{
		public:
			~CAudioSystem();

			void create();
			void update(float fDt);
		private:
			ALCdevice* pDevice{ nullptr };
			ALCcontext* pContext{ nullptr };
			int8_t contextMadeCurrent{ false };
		};
	}
}