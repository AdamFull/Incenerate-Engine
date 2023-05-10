#include "DescriptorSet.h"

#include "APIHandle.h"
#include "APICompatibility.h"

using namespace engine::graphics;

CDescriptorSet::CDescriptorSet(CDevice* device)
{
    pDevice = device;
}

CDescriptorSet::~CDescriptorSet()
{
    auto& vkDevice = pDevice->getLogical();
    if (descriptorPool)
    {
        for(auto& [set, descriptorSets] : mDescriptorSets)
            vkDevice.freeDescriptorSets(descriptorPool, descriptorSets);
    }
        
    mDescriptorSets.clear();
}

void CDescriptorSet::create(const vk::DescriptorPool& pool, const std::unordered_map<uint32_t, vk::DescriptorSetLayout>& descriptorSetLayouts)
{
    descriptorPool = pool;

    auto framesInFlight = pDevice->getFramesInFlight();

    for (auto& [set, descriptorSetLayout] : descriptorSetLayouts)
    {
        std::vector<vk::DescriptorSetLayout> vDescriptorSetLayouts(framesInFlight, descriptorSetLayout);

        vk::DescriptorSetAllocateInfo allocInfo{};
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(vDescriptorSetLayouts.size());
        allocInfo.pSetLayouts = vDescriptorSetLayouts.data();
        mDescriptorSets[set].resize(framesInFlight);

        vk::Result res = pDevice->create(allocInfo, mDescriptorSets[set].data());
        log_cerror(APICompatibility::check(res), "Cannot create descriptor sets.");
    }
}

void CDescriptorSet::update(std::vector<vk::WriteDescriptorSet>& vWrites)
{
    auto& vkDevice = pDevice->getLogical();
    log_cerror(vkDevice, "Trying to update descriptor sets, but device is invalid.");
    for (auto& write : vWrites)
        write.dstSet = get();

    vkDevice.updateDescriptorSets(static_cast<uint32_t>(vWrites.size()), vWrites.data(), 0, nullptr);
}

void CDescriptorSet::update(vk::WriteDescriptorSet& writes)
{
    auto& vkDevice = pDevice->getLogical();
    log_cerror(vkDevice, "Trying to free descriptor sets, but device is invalid.");
    writes.dstSet = get();
    vkDevice.updateDescriptorSets(1, &writes, 0, nullptr);
}

void CDescriptorSet::bind(const vk::CommandBuffer& commandBuffer, const vk::PipelineBindPoint pipelineBindPoint, const vk::PipelineLayout& pipelineLayout) const
{
    auto currentFrame = pDevice->getCurrentFrame();

    std::vector<vk::DescriptorSet> vDescriptorSets;
    for (auto& [set, descriptorSets] : mDescriptorSets)
        vDescriptorSets.emplace_back(descriptorSets.at(currentFrame));
    
    commandBuffer.bindDescriptorSets(pipelineBindPoint, pipelineLayout, 0, static_cast<uint32_t>(vDescriptorSets.size()), vDescriptorSets.data(), 0, nullptr);
}

vk::DescriptorSet& CDescriptorSet::get(uint32_t set)
{
    auto currentFrame = pDevice->getCurrentFrame();
    return mDescriptorSets[set].at(currentFrame);
}

CDevice* CDescriptorSet::getDevice()
{
    return pDevice;
}