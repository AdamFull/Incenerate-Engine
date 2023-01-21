#pragma once

namespace engine
{
	namespace ecs
	{
		class CChromaticAberrationEffect
		{
		public:
			void create();
			size_t render(bool enable, size_t in_source, size_t out_source);
		private:
			size_t shader_aberr{ invalid_index };
		};
	}
}