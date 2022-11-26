#include "UniformHandler.h"

#include "APIHandle.h"

#include <numeric>

using namespace engine::graphics;

CUniformHandler::CUniformHandler(CDevice* device)
{
	pDevice = device;
}

void CUniformHandler::create(const CUniformBlock& _uniformBlock)
{
	CHandler::create(_uniformBlock);

    for (uint32_t i = 0; i < pDevice->getFramesInFlight(); i++)
        vBuffers.emplace_back(CBuffer::MakeUniformBuffer(pDevice, uniformBlock->getSize(), 1));
}