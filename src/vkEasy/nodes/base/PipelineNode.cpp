#include <vkEasy/nodes/base/PipelineNode.h>

using namespace VK_EASY_NAMESPACE;
PipelineNode::PipelineNode(const std::string& nodeName)
    : Node(nodeName)
{
    m_updateFunction = [=]() {
        if (m_onUpdateFunction)
            m_onUpdateFunction();
        if (m_needsRebuild) {
            m_needsRebuild = false;
            buildPipeline();
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