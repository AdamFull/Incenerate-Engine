#include "Engine.h"

using namespace engine;
using namespace engine::graphics;
using namespace engine::system::window;

void CEngine::create(const FEngineCreateInfo& createInfo)
{
	pWindow = std::make_unique<CWindowHandle>();
	pWindow->create(createInfo.window);

	pGraphics = std::make_unique<CAPIHandle>();
	pGraphics->create(createInfo);
}

const winptr_t& CEngine::getWindow() const
{
	return pWindow;
}

const graphptr_t& CEngine::getGraphics() const
{
	return pGraphics;
}