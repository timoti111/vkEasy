#pragma once
#include <vkEasy/nodes/base/PipelineNode.h>

namespace VK_EASY_NAMESPACE {
class GraphicsNode : public PipelineNode {
    friend class Graph;

public:
    GraphicsNode(GraphicsNode const&) = delete;
    void operator=(GraphicsNode const&) = delete;

    ShaderStage& getVertexShaderStage();
    ShaderStage& getFragmentShaderStage();

protected:
    GraphicsNode();
    void update(Device* device);
    void buildPipeline(vk::easy::Device* device);

private:
    vk::PipelineColorBlendAttachmentState m_testAttachment;

    vk::PipelineVertexInputStateCreateInfo m_vertexInputState;
    vk::PipelineInputAssemblyStateCreateInfo m_inputAssemblyState;
    vk::PipelineTessellationStateCreateInfo m_tessellationState;
    vk::PipelineViewportStateCreateInfo m_viewportState;
    vk::PipelineRasterizationStateCreateInfo m_rasterizationState;
    vk::PipelineMultisampleStateCreateInfo m_multisampleState;
    vk::PipelineDepthStencilStateCreateInfo m_depthStencilState;
    vk::PipelineColorBlendStateCreateInfo m_colorBlendState;
    vk::PipelineDynamicStateCreateInfo m_dynamicState;
    std::vector<vk::DynamicState> m_dynamicStates;
    vk::RenderPassBeginInfo m_renderPassBeginInfo;

    vk::RenderPassCreateInfo m_renderPassCreateInfo;
    std::unique_ptr<vk::raii::RenderPass> m_renderPass;

    std::set<Resource*> m_colorAttachments;

    vk::GraphicsPipelineCreateInfo m_graphicsPipelineCreateInfo;
};
} // namespace VK_EASY_NAMESPACE