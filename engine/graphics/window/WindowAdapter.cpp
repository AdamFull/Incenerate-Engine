#include "WindowAdapter.h"

using namespace engine::graphics;

void IWindowAdapter::getWindowSize(int32_t* width, int32_t* height)
{
	*width = iWidth;
	*height = iHeight;
}

float IWindowAdapter::getAspect()
{
	return static_cast<float>(iWidth) / static_cast<float>(iHeight);
}

int32_t IWindowAdapter::getWidth()
{
	return iWidth;
}

int32_t IWindowAdapter::getHeight()
{
	return iHeight;
}