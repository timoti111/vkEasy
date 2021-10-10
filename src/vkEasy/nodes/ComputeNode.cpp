#include <vkEasy/Graph.h>
#include <vkEasy/nodes/ComputeNode.h>

using namespace VK_EASY_NAMESPACE;

ComputeNode::ComputeNode()
    : PipelineNode("ComputeNode")
{
}

ShaderStage* ComputeNode::getShaderStage()
{
}

void ComputeNode::onUpdate(std::function<void(ComputeNode&)> update)
{
    m_onUpdateFunction = [update, this]() { update(*this); };
}

void ComputeNode::setDispatchSize(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
}

void ComputeNode::buildPipeline(vk::raii::Device* device)
{
    m_computePipelineCreateInfo.setLayout(**m_pipelineLayout).setStage(getShaderStage()->update(*device));
}