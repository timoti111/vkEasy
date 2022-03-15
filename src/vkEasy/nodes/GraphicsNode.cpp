#include <vkEasy/Device.h>
#include <vkEasy/nodes/GraphicsNode.h>

using namespace VK_EASY_NAMESPACE;

GraphicsNode::GraphicsNode()
    : PipelineNode("GraphicsNode")
{
    m_dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eLineWidth };

    m_graphicsPipelineCreateInfo.setPVertexInputState(&m_vertexInputState)
        .setPInputAssemblyState(&m_inputAssemblyState)
        .setPTessellationState(&m_tessellationState)
        .setPViewportState(&m_viewportState)
        .setPRasterizationState(&m_rasterizationState)
        .setPMultisampleState(&m_multisampleState)
        .setPDepthStencilState(&m_depthStencilState)
        .setPColorBlendState(&m_colorBlendState)
        .setPDynamicState(&m_dynamicState);

    m_inputAssemblyState.setTopology(vk::PrimitiveTopology::eTriangleList);
    m_rasterizationState.setLineWidth(1.0f)
        .setCullMode(vk::CullModeFlagBits::eBack)
        .setFrontFace(vk::FrontFace::eClockwise);
    m_multisampleState.setMinSampleShading(1.0f);
    m_testAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
        | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    m_colorBlendState.setAttachments(m_testAttachment);
    m_dynamicState.setDynamicStates(m_dynamicStates);
}

void GraphicsNode::update(Device* device)
{
    m_basePipelineUpdateFunction(device);

    auto graphicsBuffers = device->getUniversalCommandBuffers(1);
    if (graphicsBuffers.empty())
        return;

    graphicsBuffers[0]->beginRenderPass(m_renderPassBeginInfo, vk::SubpassContents::eInline);
    graphicsBuffers[0]->bindPipeline(vk::PipelineBindPoint::eCompute, **m_pipeline);
    if (!m_descriptorSetsToBind.empty())
        graphicsBuffers[0]->bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics, **m_pipelineLayout, 0, m_descriptorSetsToBind, {});
    graphicsBuffers[0]->draw(3, 1, 0, 0);
    graphicsBuffers[0]->endRenderPass();
}

void GraphicsNode::buildPipeline(vk::easy::Device* device)
{
    std::vector<vk::PipelineShaderStageCreateInfo> stageInfos;
    for (auto& [key, element] : m_stages)
        stageInfos.push_back(*element->getPipelineShaderStageCreateInfo());
    m_renderPass = std::make_unique<vk::raii::RenderPass>(*device->getLogicalDevice(), m_renderPassCreateInfo);
    m_graphicsPipelineCreateInfo.setLayout(**m_pipelineLayout).setStages(stageInfos).setRenderPass(**m_renderPass);
    m_pipeline = std::make_unique<vk::raii::Pipeline>(
        *device->getLogicalDevice(), *m_pipelineCache, m_graphicsPipelineCreateInfo);
    m_renderPassBeginInfo.setRenderPass(**m_renderPass);
}

ShaderStage& GraphicsNode::getVertexShaderStage()
{
    return *getShaderStage(vk::ShaderStageFlagBits::eVertex);
}

ShaderStage& GraphicsNode::getFragmentShaderStage()
{
    return *getShaderStage(vk::ShaderStageFlagBits::eFragment);
}
