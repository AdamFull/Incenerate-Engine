#include "engine/engine.h"
#include "engine/graphics/image/Image.h"

#include <random>
#include <limits>

using namespace engine;
using namespace engine::graphics;

int main()
{
	CEngine::getInstance()->create();
	CEngine::getInstance()->beginEngineLoop();

	return 0;
}