#include <iostream>
#include <vkEasy/Device.h>
#include <vkEasy/Graph.h>
#include <vkEasy/nodes/ComputeNode.h>

using namespace VK_EASY_NAMESPACE;

ComputeNode::ComputeNode()
    : PipelineNode("ComputeNode")
{
    m_neededQueueTypes = vk::QueueFlagBits::eCompute;
    m_pipelineStage = vk::PipelineStageFlagBits::eComputeShader;
}

void ComputeNode::update(Device* device)
{
    m_basePipelineUpdateFunction(device);

    auto computeBuffers = device->getUniversalCommandBuffers(1);
    if (computeBuffers.empty())
        return;

    computeBuffers[0]->bindPipeline(vk::PipelineBindPoint::eCompute, **m_pipeline);
    computeBuffers[0]->bindDescriptorSets(
        vk::PipelineBindPoint::eCompute, **m_pipelineLayout, 0, m_descriptorSetsToBind, {});
    computeBuffers[0]->dispatch(m_dispatchSize[0], m_dispatchSize[1], m_dispatchSize[2]);
}

ShaderStage& ComputeNode::getComputeShaderStage()
{
    return *getShaderStage(vk::ShaderStageFlagBits::eCompute);
}

void ComputeNode::setDispatchSize(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
    m_dispatchSize = { groupCountX, groupCountY, groupCountZ };
}

void ComputeNode::buildPipeline(vk::easy::Device* device)
{
    m_computePipelineCreateInfo.setLayout(**m_pipelineLayout)
        .setStage(*getComputeShaderStage().getPipelineShaderStageCreateInfo());
    m_pipeline = std::make_unique<vk::raii::Pipeline>(
        *device->getLogicalDevice(), *m_pipelineCache, m_computePipelineCreateInfo);
}