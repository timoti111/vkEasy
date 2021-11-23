#include <iostream>
#include <vkEasy/Device.h>
#include <vkEasy/Graph.h>
#include <vkEasy/nodes/ComputeNode.h>

using namespace VK_EASY_NAMESPACE;

ComputeNode::ComputeNode()
    : PipelineNode("ComputeNode")
{
    m_pipelineStage = vk::PipelineStageFlagBits::eComputeShader;
    m_shaderStage = createShaderStage(vk::ShaderStageFlagBits::eCompute);
    m_updateFunction = [this](Device* device) {
        m_basePipelineUpdateFunction(device);

        auto computeBuffers = device->getComputeCommandBuffers(1);
        if (computeBuffers.empty())
            return;

        std::cout << "Executing: " << objectName() << std::endl;
        computeBuffers[0]->bindPipeline(vk::PipelineBindPoint::eCompute, **m_pipeline);
        computeBuffers[0]->bindDescriptorSets(
            vk::PipelineBindPoint::eCompute, **m_pipelineLayout, 0, m_descriptorSetsToBind, {});
        computeBuffers[0]->dispatch(m_dispatchSize[0], m_dispatchSize[1], m_dispatchSize[2]);
    };
}

ShaderStage* ComputeNode::getShaderStage()
{
    return m_shaderStage;
}

void ComputeNode::onUpdate(std::function<void(ComputeNode&)> update)
{
    m_onUpdateFunction = [update, this]() { update(*this); };
}

void ComputeNode::setDispatchSize(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
    m_dispatchSize = { groupCountX, groupCountY, groupCountZ };
}

void ComputeNode::buildPipeline(vk::easy::Device* device)
{
    vk::PipelineCacheCreateInfo pipelineCacheCreateInfo;
    m_pipelineCache = std::make_unique<vk::raii::PipelineCache>(*device->getLogicalDevice(), pipelineCacheCreateInfo);
    m_shaderStage->update(*device->getLogicalDevice());
    m_computePipelineCreateInfo.setLayout(**m_pipelineLayout)
        .setStage(*m_shaderStage->getPipelineShaderStageCreateInfo());
    m_pipeline = std::make_unique<vk::raii::Pipeline>(
        *device->getLogicalDevice(), *m_pipelineCache, m_computePipelineCreateInfo);
}