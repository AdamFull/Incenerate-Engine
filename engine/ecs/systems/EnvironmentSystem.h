#pragma once

#include "ecs/core/System.hpp"

namespace engine
{
	namespace ecs
	{
		class CEnvironmentSystem : public ISystem
		{
		public:
			virtual ~CEnvironmentSystem() override = default;

			void __create() override;
			void __update(float fDt) override;
		private:
			size_t computeBRDFLUT(uint32_t size);
			size_t computeIrradiance(size_t origin, uint32_t size);
			size_t computePrefiltered(size_t origin, uint32_t size);
		private:
			size_t stageId{ invalid_index };
		};
	}
}