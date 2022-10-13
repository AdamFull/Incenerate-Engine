#include "APIHandle.h"

using namespace engine::graphics;

void CAPIHandle::create(const FEngineCreateInfo& createInfo)
{
	eAPI = createInfo.eAPI;

	pDevice = std::make_unique<CDevice>();
	pDevice->create(createInfo);
}

void CAPIHandle::begin()
{

}

void CAPIHandle::end()
{

}