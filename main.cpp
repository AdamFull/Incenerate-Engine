#include "engine/engine.h"
#include "engine/system/window/WindowHandle.h"
#include "engine/render/APIHandle.h"
#include "engine/render/ProgramLoader.h"

using namespace engine;
using namespace engine::system::window;
using namespace engine::render;

int main()
{
    FEngineCreateInfo ci;
    ci.eAPI = ERenderApi::eVulkan;

    ci.window.width = 800;
    ci.window.height = 600;
    ci.window.srName = "my window";

    CEngine::getInstance()->create(ci);

    auto& window = CEngine::getInstance()->getWindow();
    auto& render = CEngine::getInstance()->getRender();

    CProgramLoader::getInstance()->load("test_shader");

    while (window->begin())
    {
        render->begin();

        render->end();
        window->end();
    }

    return 0;
}