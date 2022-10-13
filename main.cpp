#include "engine/engine.h"

using namespace engine;

int main()
{
    FEngineCreateInfo ci;
    ci.eAPI = ERenderApi::eVulkan_1_0;

    ci.window.width = 800;
    ci.window.height = 600;
    ci.window.srName = "my window";

    CEngine::getInstance()->create(ci);

    auto& window = CEngine::getInstance()->getWindow();
    auto& graphics = CEngine::getInstance()->getGraphics();

    while (window->begin())
    {
        window->end();
    }

    return 0;
}