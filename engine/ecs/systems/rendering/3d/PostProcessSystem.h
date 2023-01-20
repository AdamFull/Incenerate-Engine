#pragma once

#include <vulkan/vulkan.hpp>
#include "ecs/systems/BaseSystem.h"
#include "event/Event.hpp"

namespace engine
{
	namespace ecs
	{
		class CPostProcessSystem : public ISystem
		{
		public:
			CPostProcessSystem() { name = "Rendering post process system"; }
			virtual ~CPostProcessSystem() override;

			void __create() override;
			void __update(float fDt) override;
		private:
			void onViewportChanged(CEvent& event);
			size_t createImage(const std::string& name, vk::Format format);
			void addSubresource(const std::string& name);
			size_t getSubresource(const std::string& name);

			std::map<std::string, std::vector<size_t>> mSubresourceMap;

			size_t shader_id_tonemap{ invalid_index };
			size_t shader_id_downsample{ invalid_index };
			size_t shader_id_blur{ invalid_index };
			size_t bloom_image{ invalid_index };
			size_t bloom_image2{ invalid_index };
		};
	}
}