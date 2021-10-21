#include <vkEasy/Graph.h>
#include <vkEasy/nodes/base/PipelineNode.h>

using namespace VK_EASY_NAMESPACE;
PipelineNode::PipelineNode(const std::string& nodeName)
    : Node(nodeName)
{
    m_updateFunction = [this](vk::easy::Device* device) {
        if (m_onUpdateFunction)
            m_onUpdateFunction();
        if (m_needsRebuild) {
            m_needsRebuild = false;
            buildPipeline(device);
        }
    };
}

ShaderStage& PipelineNode::createShaderStage(const vk::ShaderStageFlagBits& stage)
{
}

void PipelineNode::needsRebuild()
{
    m_needsRebuild = true;
}