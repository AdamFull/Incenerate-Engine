#include "QueryPool.h"

#include "graphics/Device.h"

using namespace engine::graphics;

CQueryPool::CQueryPool(CDevice* device)
{
	pDevice = device;
}

CQueryPool::~CQueryPool()
{
	pDevice->destroy(&queryPool);
}

void CQueryPool::create()
{
	vk::QueryPoolCreateInfo createInfo{};
	createInfo.queryType = vk::QueryType::eOcclusion;
	createInfo.queryCount = 10000u;
	auto res = pDevice->create(createInfo, &queryPool);
	log_cerror(VkHelper::check(res), "Cannot create query pool.");
}

void CQueryPool::clear(vk::CommandBuffer& commandBuffer)
{
	commandBuffer.resetQueryPool(queryPool, 0u, 10000u);
	vPixelDrawn.clear();
	mAssociation.clear();
}

void CQueryPool::begin(vk::CommandBuffer& commandBuffer, uint32_t index)
{
	mAssociation.emplace(index, mAssociation.size());
	commandBuffer.beginQuery(queryPool, mAssociation[index], vk::QueryControlFlags{});
}

void CQueryPool::end(vk::CommandBuffer& commandBuffer, uint32_t index)
{
	commandBuffer.endQuery(queryPool, mAssociation[index]);
}

void CQueryPool::takeResult()
{
	auto& device = pDevice->getLogical();
	auto elements = mAssociation.size();
	auto elementSize = sizeof(uint64_t);
	auto dataSize = elements * elementSize;
	vk::QueryResultFlags flags = vk::QueryResultFlagBits::e64 | vk::QueryResultFlagBits::eWait;

	auto result = device.getQueryPoolResults<uint64_t>(queryPool, 0u, elements, dataSize, static_cast<vk::DeviceSize>(elementSize), flags);
	log_cerror(VkHelper::check(result.result), "Cannot create query pool.");
	vPixelDrawn = result.value;
}

bool CQueryPool::wasDrawn(uint32_t index)
{
	auto it = mAssociation.find(index);
	if (it != mAssociation.end())
	{
		auto& count = vPixelDrawn.at(it->second);
		return count > 0;
	}
	
	return false;
}

CDevice* CQueryPool::getDevice()
{
	return pDevice;
}