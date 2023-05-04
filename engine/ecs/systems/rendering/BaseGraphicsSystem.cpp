#include "BaseGraphicsSystem.h"

#include "Engine.h"

using namespace engine::ecs;

void CBaseGraphicsSystem::create()
{
	graphics = EGEngine->getGraphics().get();
	ISystem::create();
}

void CBaseGraphicsSystem::__create()
{
	EGEngine->addEventListener(Events::Graphics::ReCreate, this, &CBaseGraphicsSystem::onViewportUpdated);
	updateSubresources();
}

void CBaseGraphicsSystem::__update(float fDt)
{
}

void CBaseGraphicsSystem::onViewportUpdated(const std::unique_ptr<IEvent>& event)
{
	mSubresourceMap.clear();
	updateSubresources();
}


void CBaseGraphicsSystem::addSubresource(const std::string& name)
{
	vSubresourceNames.push_back(name);
}

size_t CBaseGraphicsSystem::getSubresource(const std::string& name)
{
	auto& device = graphics->getDevice();
	auto index = device->getCurrentFrame();

	return mSubresourceMap[name].at(index);
}

void CBaseGraphicsSystem::updateSubresources()
{
	auto& device = graphics->getDevice();
	auto framesInFlight = device->getFramesInFlight();

	for (auto& name : vSubresourceNames)
	{
		for (uint32_t i = 0; i < framesInFlight; i++)
			mSubresourceMap[name].emplace_back(graphics->getImageID(name + "_" + std::to_string(i)));
	}
}