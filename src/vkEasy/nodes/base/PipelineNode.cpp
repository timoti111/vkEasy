#include <vkEasy/Device.h>
#include <vkEasy/Graph.h>
#include <vkEasy/nodes/base/PipelineNode.h>
#include <vkEasy/resources/base/Buffer.h>

using namespace VK_EASY_NAMESPACE;
PipelineNode::PipelineNode(const std::string& nodeName)
    : Node(nodeName)
{
    m_basePipelineUpdateFunction = [this](Device* device) {
        if (m_pipelineRebuild)
            build(device);

        if (m_needsRebuild || m_pipelineRebuild) {
            vk::PipelineCacheCreateInfo pipelineCacheCreateInfo;
            m_pipelineCache
                = std::make_unique<vk::raii::PipelineCache>(*device->getLogicalDevice(), pipelineCacheCreateInfo);
            for (auto& [key, element] : m_stages)
                element->update(device);
            buildPipeline(device);
        }

        m_needsRebuild = false;
        m_pipelineRebuild = false;
    };
}

void PipelineNode::build(Device* device)
{
    m_poolSizes.clear();
    m_setLayouts.clear();
    m_writeDescriptorSets.clear();
    m_descriptorSetsToBind.clear();
    m_setLayouts.clear();
    m_setLayoutsVk.clear();

    std::map<vk::DescriptorType, size_t> poolSizes;

    for (size_t set = 0; set < m_layout.rbegin()->first + 1; set++) {
        vk::DescriptorSetLayoutCreateInfo descriptorLayout;
        std::vector<vk::DescriptorSetLayoutBinding> bindings;
        if (m_layout.find(set) != m_layout.end()) {
            for (size_t binding = 0; binding < m_layout[set].rbegin()->first + 1; binding++) {
                vk::DescriptorSetLayoutBinding layoutBinding;
                layoutBinding.setBinding(binding);
                if (m_layout[set].find(binding) != m_layout[set].end()) {
                    auto& descriptor = m_layout[set][binding];
                    layoutBinding.setDescriptorType(descriptor.type)
                        .setDescriptorCount(descriptor.resources.size())
                        .setStageFlags(descriptor.shaderStageFlags);
                    if (poolSizes.find(descriptor.type) == poolSizes.end())
                        poolSizes[descriptor.type] = 0;
                    poolSizes[descriptor.type] += descriptor.resources.size();
                }
                bindings.emplace_back(layoutBinding);
            }
        }
        descriptorLayout.setBindings(bindings);
        m_setLayouts.emplace_back(
            std::make_unique<vk::raii::DescriptorSetLayout>(*device->getLogicalDevice(), descriptorLayout));
    }

    for (auto& setLayout : m_setLayouts)
        m_setLayoutsVk.push_back(**setLayout.get());

    for (auto& [type, size] : poolSizes)
        if (size > 0)
            m_poolSizes.push_back(vk::DescriptorPoolSize().setType(type).setDescriptorCount(size));

    vk::DescriptorPoolCreateInfo descriptorPoolInfo;
    descriptorPoolInfo.setPoolSizes(m_poolSizes)
        .setMaxSets(m_setLayouts.size())
        .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
    m_descriptorPool = std::make_unique<vk::raii::DescriptorPool>(*device->getLogicalDevice(), descriptorPoolInfo);

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    pipelineLayoutCreateInfo.setSetLayouts(m_setLayoutsVk);
    m_pipelineLayout
        = std::make_unique<vk::raii::PipelineLayout>(*device->getLogicalDevice(), pipelineLayoutCreateInfo);

    vk::DescriptorSetAllocateInfo allocInfo;
    allocInfo.setDescriptorPool(**m_descriptorPool).setSetLayouts(m_setLayoutsVk);
    m_descriptorSets = std::make_unique<vk::raii::DescriptorSets>(*device->getLogicalDevice(), allocInfo);

    for (auto& descriptorSet : *m_descriptorSets)
        m_descriptorSetsToBind.push_back(*descriptorSet);

    for (auto& [set, bindingDescriptor] : m_layout) {
        for (auto& [binding, descriptor] : bindingDescriptor) {
            vk::WriteDescriptorSet writeDescriptorSet;
            writeDescriptorSet.setDstSet(m_descriptorSetsToBind.at(set))
                .setDstBinding(binding)
                .setDescriptorType(descriptor.type)
                .setDescriptorCount(descriptor.resources.size());

            descriptor.bufferInfos.clear();
            for (auto& resource : descriptor.resources) {
                vk::DescriptorBufferInfo bufferDescriptor;
                bufferDescriptor.setOffset(0).setRange(VK_WHOLE_SIZE);
                if (resource->isBuffer()) {
                    bufferDescriptor.setBuffer(dynamic_cast<Buffer*>(resource)->getVkBuffer());
                    descriptor.bufferInfos.push_back(bufferDescriptor);
                }
            }

            if (!descriptor.bufferInfos.empty())
                writeDescriptorSet.setBufferInfo(descriptor.bufferInfos);

            m_writeDescriptorSets.emplace_back(writeDescriptorSet);
        }
    }

    device->getLogicalDevice()->updateDescriptorSets(m_writeDescriptorSets, {});
}

Descriptor* PipelineNode::createDescriptor(std::vector<Resource*> resources, size_t binding, size_t set)
{
    m_pipelineRebuild = true;
    Descriptor descriptor;
    for (auto& resource : resources) {
        Node::uses(resource);
        descriptor.type = resource->getDescriptorType();
    }
    descriptor.resources = resources;
    m_layout[set][binding] = std::move(descriptor);
    return &m_layout[set][binding];
}

ShaderStage* PipelineNode::getShaderStage(const vk::ShaderStageFlagBits& stage)
{
    if (!m_stages[stage])
        m_stages[stage] = std::unique_ptr<ShaderStage>(new ShaderStage(stage, this));
    return m_stages[stage].get();
}

void PipelineNode::needsRebuild()
{
    m_needsRebuild = true;
}