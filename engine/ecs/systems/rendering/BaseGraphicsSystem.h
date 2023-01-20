#pragma once

#include "ecs/systems/BaseSystem.h"
#include "event/Event.hpp"

namespace engine
{
	namespace ecs
	{
		class CBaseGraphicsSystem : public ISystem
		{
		public:
			CBaseGraphicsSystem() { name = "Base graphics"; }
			virtual ~CBaseGraphicsSystem() override = default;

			virtual void __create() override;
			virtual void __update(float fDt) override;
		protected:
			virtual void onViewportUpdated(CEvent& event);
			void addSubresource(const std::string& name);
			size_t getSubresource(const std::string& name);

		private:
			void updateSubresources();
			std::vector<std::string> vSubresourceNames;
			std::map<std::string, std::vector<size_t>> mSubresourceMap;
		};
	}
}