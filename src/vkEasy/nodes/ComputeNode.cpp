#include <vkEasy/nodes/ComputeNode.h>

using namespace VK_EASY_NAMESPACE;

ShaderStage* ComputeNode::getShaderStage()
{
}

void ComputeNode::onUpdate(std::function<void(ComputeNode&)> update)
{
}

void ComputeNode::setDispatchSize(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
}

void ComputeNode::buildPipeline()
{
}