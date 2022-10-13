#include "UniformHandler.h"

#include "Device.h"

#include <numeric>

using namespace engine::graphics;

CUniformHandler::CUniformHandler(CDevice* device)
{
	pDevice = device;
}

void CUniformHandler::create(const CUniformBlock& _uniformBlock)
{
	CHandler::create(_uniformBlock);
    auto framesInFlight = pDevice->getFramesInFlight();

    auto physProps = pDevice->getPhysical().getProperties();
    auto minOffsetAllignment = std::lcm(physProps.limits.minUniformBufferOffsetAlignment, physProps.limits.nonCoherentAtomSize);

    for (uint32_t i = 0; i < framesInFlight; i++)
    {
        auto uniform = std::make_unique<CBuffer>(pDevice);
        uniform->create(uniformBlock->getSize(), 1,
            vk::BufferUsageFlagBits::eUniformBuffer, 
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            minOffsetAllignment);
        vBuffers.emplace_back(std::move(uniform));
    }
}