#include "DescriptorSet.h"

#include "Device.h"

using namespace engine::graphics;

CDescriptorSet::CDescriptorSet(CDevice* device)
{
	pDevice = device;
}

CDescriptorSet::~CDescriptorSet()
{
    auto& vkDevice = pDevice->getLogical();
    if (!descriptorPool)
        vkDevice.freeDescriptorSets(descriptorPool, vDescriptorSets);
    vDescriptorSets.clear();
}

void CDescriptorSet::create(vk::PipelineBindPoint bindPoint, vk::PipelineLayout& layout, vk::DescriptorPool& pool, vk::DescriptorSetLayout& setLayout)
{
    pipelineBindPoint = bindPoint;
    pipelineLayout = layout;
    descriptorPool = pool;

    auto framesInFlight = pDevice->getFramesInFlight();
    std::vector<vk::DescriptorSetLayout> vSetLayouts(framesInFlight, setLayout);
    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = framesInFlight;
    allocInfo.pSetLayouts = vSetLayouts.data();
    vDescriptorSets.resize(framesInFlight);

    vk::Result res = pDevice->create(allocInfo, vDescriptorSets.data());
    assert(res == vk::Result::eSuccess && "Cannot create descriptor sets.");
}

void CDescriptorSet::update(std::vector<vk::WriteDescriptorSet>& vWrites)
{
    auto& vkDevice = pDevice->getLogical();
    assert(vkDevice && "Trying to update descriptor sets, but device is invalid.");
    for (auto& write : vWrites)
        write.dstSet = get();

    vkDevice.updateDescriptorSets(static_cast<uint32_t>(vWrites.size()), vWrites.data(), 0, nullptr);
}

void CDescriptorSet::update(vk::WriteDescriptorSet& writes)
{
    auto& vkDevice = pDevice->getLogical();
    assert(vkDevice && "Trying to free descriptor sets, but device is invalid.");
    writes.dstSet = get();
    vkDevice.updateDescriptorSets(1, &writes, 0, nullptr);
}

void CDescriptorSet::bind(const vk::CommandBuffer& commandBuffer) const
{
    auto currentFrame = pDevice->getCurrentFrame();
    commandBuffer.bindDescriptorSets(pipelineBindPoint, pipelineLayout, 0, 1, &vDescriptorSets.at(currentFrame), 0, nullptr);
}

vk::DescriptorSet& CDescriptorSet::get()
{
    auto currentFrame = pDevice->getCurrentFrame();
    return vDescriptorSets.at(currentFrame);
}