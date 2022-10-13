#include "Handler.h"

#include "Device.h"

using namespace engine::graphics;

CHandler::CHandler(CDevice* render)
{

}

void CHandler::create(const CUniformBlock& uniformBlock)
{
	auto framesInFlight = pDevice->getFramesInFlight();
	this->uniformBlock = uniformBlock;
	vMapped.resize(framesInFlight);
}

void CHandler::reCreate()
{
	create(uniformBlock.value());
}

void CHandler::flush()
{
    auto index = getCurrentFrameProxy();
    if (vBuffers.empty())
        return;

    if (status != EHandlerStatus::eFlushed)
    {
        if (vMapped.at(index))
        {
            vBuffers.at(index)->unmap();
            vMapped.at(index) = false;
        }

        status = EHandlerStatus::eFlushed;
    }
}

const std::unique_ptr<CBuffer>& CHandler::getBuffer()
{
    auto index = getCurrentFrameProxy();
    return vBuffers.at(index);
}

uint32_t CHandler::getCurrentFrameProxy()
{
    return pDevice->getCurrentFrame();
}