#include "BindlessDescriptor.h"

#include "Device.h"
#include "APIHandle.h"
#include "APICompatibility.h"

using namespace engine::graphics;

static const uint32_t k_max_bindless_resources = 16536;

CBindlessDescriptor::CBindlessDescriptor(CDevice* device) :
    pDevice(device)
{

}

CBindlessDescriptor::~CBindlessDescriptor()
{
    auto& vkDevice = pDevice->getLogical();

    if (m_pDescriptorPool)
        vkDevice.freeDescriptorSets(m_pDescriptorPool, m_vDescriptorSets);
    if (m_pDescriptorSetLayout)
        pDevice->destroy(&m_pDescriptorSetLayout);
    if (m_pDescriptorPool)
        pDevice->destroy(&m_pDescriptorPool);
}

void CBindlessDescriptor::create()
{
    createDescriptorPool();
    createDescriptorSetLayout();
    createDescriptorSet();
}

void CBindlessDescriptor::update()
{
    auto graphics = pDevice->getAPI();
    auto currentFrame = pDevice->getCurrentFrame();
    auto& texQueue = m_mTextureIds[currentFrame];
    if (texQueue.empty())
        return;

    std::vector<vk::WriteDescriptorSet> writeDescriptorSets{};

    while (!texQueue.empty())
    {
        auto tex_id = texQueue.front();
        auto& image = graphics->getImage(tex_id);

        writeDescriptorSets.emplace_back(vk::WriteDescriptorSet{});

        auto& writeDescriptorSet = writeDescriptorSets.back();
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.dstArrayElement = tex_id;
        writeDescriptorSet.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        writeDescriptorSet.dstSet = m_vDescriptorSets.at(currentFrame);
        writeDescriptorSet.dstBinding = 0;
        writeDescriptorSet.pImageInfo = &image->getDescriptor();

        texQueue.pop();
    }

    auto& vkDevice = pDevice->getLogical();
    log_cerror(vkDevice, "Trying to update descriptor sets, but device is invalid.");
    vkDevice.updateDescriptorSets(static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
}

void CBindlessDescriptor::addTexture(size_t tex_id)
{
    auto framesInFlight = pDevice->getFramesInFlight();

    for (uint32_t frame = 0; frame < framesInFlight; ++frame)
    {
        m_mTextureIds[frame].push(tex_id);
    }
}

void CBindlessDescriptor::bind(const vk::CommandBuffer& commandBuffer, const vk::PipelineBindPoint pipelineBindPoint, const vk::PipelineLayout& pipelineLayout)
{
    // TODO: 2 - is "set = 2"
    auto currentFrame = pDevice->getCurrentFrame();
    commandBuffer.bindDescriptorSets(pipelineBindPoint, pipelineLayout, 1, 1, &m_vDescriptorSets.at(currentFrame), 0, nullptr);
}

const vk::DescriptorSet& CBindlessDescriptor::get() const
{
    auto currentFrame = pDevice->getCurrentFrame();
    return m_vDescriptorSets.at(currentFrame);
}

const vk::DescriptorSetLayout& CBindlessDescriptor::getDescriptorSetLayout() const
{
    return m_pDescriptorSetLayout;
}

const vk::DescriptorPool& CBindlessDescriptor::getDescriptorPool() const
{
    return m_pDescriptorPool;
}

void CBindlessDescriptor::createDescriptorPool()
{
    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes{ {vk::DescriptorType::eCombinedImageSampler, k_max_bindless_resources } };

    auto framesInFlight = pDevice->getFramesInFlight();

    uint32_t maxSets{ 0u };
    for (auto& poolSize : descriptorPoolSizes)
    {
        poolSize.descriptorCount *= framesInFlight;
        maxSets += poolSize.descriptorCount;
    }

    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
    descriptorPoolCreateInfo.flags = vk::DescriptorPoolCreateFlagBits::eUpdateAfterBindEXT;
    descriptorPoolCreateInfo.maxSets = maxSets;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
    descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
    vk::Result res = pDevice->create(descriptorPoolCreateInfo, &m_pDescriptorPool);
    log_cerror(APICompatibility::check(res), "Cannot create descriptor pool.");
}

void CBindlessDescriptor::createDescriptorSetLayout()
{
    vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding{};
    descriptorSetLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    descriptorSetLayoutBinding.descriptorCount = k_max_bindless_resources;
    descriptorSetLayoutBinding.binding = 0;
    descriptorSetLayoutBinding.stageFlags = vk::ShaderStageFlagBits::eAllGraphics;
    descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

    vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCI{};
    descriptorSetLayoutCI.bindingCount = 1;
    descriptorSetLayoutCI.pBindings = &descriptorSetLayoutBinding;
    descriptorSetLayoutCI.flags = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPoolEXT;

    vk::DescriptorBindingFlags indexingFlags{ vk::DescriptorBindingFlagBits::ePartiallyBound | vk::DescriptorBindingFlagBits::eVariableDescriptorCount | vk::DescriptorBindingFlagBits::eUpdateAfterBind };
    vk::DescriptorSetLayoutBindingFlagsCreateInfoEXT descriptorSetLayoutBindingFlagsCIEXT{};
    descriptorSetLayoutBindingFlagsCIEXT.bindingCount = 1;
    descriptorSetLayoutBindingFlagsCIEXT.pBindingFlags = &indexingFlags;

    descriptorSetLayoutCI.pNext = &descriptorSetLayoutBindingFlagsCIEXT;

    vk::Result res = pDevice->create(descriptorSetLayoutCI, &m_pDescriptorSetLayout);
    log_cerror(APICompatibility::check(res), "Cannot create descriptor set layout.");
}

void CBindlessDescriptor::createDescriptorSet()
{
    auto framesInFlight = pDevice->getFramesInFlight();
    std::vector<vk::DescriptorSetLayout> vDescriptorSetLayouts(framesInFlight, m_pDescriptorSetLayout);
    std::vector<uint32_t> vMaxSets(framesInFlight, k_max_bindless_resources - 1);
    m_vDescriptorSets.resize(framesInFlight);

    auto descriptorSetCount = static_cast<uint32_t>(vDescriptorSetLayouts.size());

    vk::DescriptorSetAllocateInfo descriptorSetAI{};
    descriptorSetAI.descriptorPool = m_pDescriptorPool;
    descriptorSetAI.descriptorSetCount = descriptorSetCount;
    descriptorSetAI.pSetLayouts = vDescriptorSetLayouts.data();

    vk::DescriptorSetVariableDescriptorCountAllocateInfoEXT descriptorSetVariableDescriptorCountAIEXT{};
    descriptorSetVariableDescriptorCountAIEXT.descriptorSetCount = descriptorSetCount;
    descriptorSetVariableDescriptorCountAIEXT.pDescriptorCounts = vMaxSets.data();;

    descriptorSetAI.pNext = &descriptorSetVariableDescriptorCountAIEXT;

    vk::Result res = pDevice->create(descriptorSetAI, m_vDescriptorSets.data());
    log_cerror(APICompatibility::check(res), "Cannot create descriptor sets.");
}