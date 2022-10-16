#include "StorageHandler.h"

#include "Device.h"

#include <numeric>

using namespace engine::graphics;

CStorageHandler::CStorageHandler(CDevice* device)
{
	pDevice = device;
}

void CStorageHandler::create(const CUniformBlock& _uniformBlock)
{
	CHandler::create(_uniformBlock);

    for (uint32_t i = 0; i < pDevice->getFramesInFlight(); i++)
        vBuffers.emplace_back(CBuffer::MakeStorageBuffer(pDevice, uniformBlock->getSize(), 1));
}