#pragma once

#include "audio/AudioReader.h"

namespace engine
{
	namespace loaders
	{
		class CAudioLoader
		{
		public:
			static std::unique_ptr<CAudioReader> load(const std::filesystem::path& filepath);
		};
	}
}