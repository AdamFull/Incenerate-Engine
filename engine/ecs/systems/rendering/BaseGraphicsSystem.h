#pragma once

#include "ecs/systems/BaseSystem.h"
#include "event/interface/EventInterface.h"

namespace engine
{
	namespace ecs
	{
		class CBaseGraphicsSystem : public ISystem
		{
		public:
			CBaseGraphicsSystem() { name = "Base graphics"; }
			virtual ~CBaseGraphicsSystem() override = default;

			virtual void create() override;
			virtual void __create() override;
			virtual void __update(float fDt) override;
		protected:
			virtual void onViewportUpdated(const std::unique_ptr<IEvent>& event);
			void addSubresource(const std::string& name);
			size_t getSubresource(const std::string& name);

		protected:
			graphics::CAPIHandle* graphics{ nullptr };
		private:
			void updateSubresources();
			std::vector<std::string> vSubresourceNames;
			std::unordered_map<std::string, std::vector<size_t>> mSubresourceMap;

		};
	}
}