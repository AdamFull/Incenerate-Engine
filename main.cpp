#include "engine/engine.h"
#include "engine/system/filesystem/filesystem.h"
#include "engine/graphics/image/Image.h"

#include <random>
#include <limits>

using namespace engine;
using namespace engine::graphics;
using namespace engine::system;

int main()
{
	CEngine::getInstance()->create();
	CEngine::getInstance()->beginEngineLoop();

	return 0;
}