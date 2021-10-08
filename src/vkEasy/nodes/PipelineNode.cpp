#include <vkEasy/nodes/PipelineNode.h>

using namespace VK_EASY_NAMESPACE;

ShaderStage& PipelineNode::createShaderStage(const vk::ShaderStageFlagBits& stage)
{
}

void PipelineNode::readsFrom(const vk::ArrayProxyNoTemporaries<Resource>& resources, uint32_t binding)
{
}

void PipelineNode::writesTo(const vk::ArrayProxyNoTemporaries<Resource>& resources, uint32_t binding)
{
}

void PipelineNode::needsUpdate()
{
}