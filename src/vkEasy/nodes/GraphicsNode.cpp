#include <vkEasy/Device.h>
#include <vkEasy/nodes/GraphicsNode.h>

using namespace VK_EASY_NAMESPACE;

GraphicsNode::GraphicsNode()
    : PipelineNode("GraphicsNode")
{
    // m_dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eLineWidth };

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

    m_subpassDescription.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
}

void GraphicsNode::update()
{
    m_basePipelineUpdateFunction();

    auto graphicsBuffers = getCommandBuffers(1);
    if (graphicsBuffers.empty())
        return;

    m_framebuffer->begin(graphicsBuffers[0]);
    graphicsBuffers[0]->bindPipeline(vk::PipelineBindPoint::eGraphics, **m_pipeline);
    if (!m_descriptorSetsToBind.empty())
        graphicsBuffers[0]->bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics, **m_pipelineLayout, 0, m_descriptorSetsToBind, {});
    graphicsBuffers[0]->draw(3, 1, 0, 0);
    m_framebuffer->end(graphicsBuffers[0]);
}

void GraphicsNode::buildPipeline()
{
    std::vector<vk::PipelineShaderStageCreateInfo> stageInfos;
    for (auto& [key, element] : m_stages)
        stageInfos.push_back(*element->getPipelineShaderStageCreateInfo());
    m_framebuffer->build();

    vk::Viewport viewport;
    viewport.setWidth(m_framebuffer->m_framebufferCreateInfo.width)
        .setHeight(m_framebuffer->m_framebufferCreateInfo.height)
        .setMaxDepth(1.0);
    vk::Rect2D scissor;
    scissor.setExtent({ m_framebuffer->m_framebufferCreateInfo.width, m_framebuffer->m_framebufferCreateInfo.height });
    m_viewportState.setViewports(viewport);
    m_viewportState.setScissors(scissor);

    m_graphicsPipelineCreateInfo.setLayout(**m_pipelineLayout)
        .setStages(stageInfos)
        .setRenderPass(**m_framebuffer->m_renderPass)
        .setSubpass(m_subpassIndex);
    m_pipeline = std::make_unique<vk::raii::Pipeline>(
        *getDevice()->getLogicalDevice(), *m_pipelineCache, m_graphicsPipelineCreateInfo);
}

ShaderStage& GraphicsNode::getVertexShaderStage()
{
    return *getShaderStage(vk::ShaderStageFlagBits::eVertex);
}

ShaderStage& GraphicsNode::getFragmentShaderStage()
{
    return *getShaderStage(vk::ShaderStageFlagBits::eFragment);
}

void GraphicsNode::setFramebuffer(Framebuffer& framebuffer)
{
    m_framebuffer = &framebuffer;
}

void GraphicsNode::inOrder()
{
    m_subpassIndex = m_framebuffer->m_references;
    m_framebuffer->m_references++;
    m_framebuffer->m_subpassNodes.push_back(this);
}

Descriptor* GraphicsNode::setInputAttachment(
    AttachmentImage* attachment, size_t attachmentIndex, size_t binding, size_t set)
{
    if (attachmentIndex + 1 > m_inputAttachments.size())
        m_inputAttachments.resize(attachmentIndex + 1);
    m_inputAttachments[attachmentIndex].setAttachment(attachment->getIndex());
    m_subpassDescription.setInputAttachments(m_inputAttachments);
    return createDescriptor({ attachment }, binding, set);
}

void GraphicsNode::setColorAttachment(AttachmentImage* attachment, size_t layout)
{
    if (layout + 1 > m_colorAttachments.size())
        m_colorAttachments.resize(layout + 1);
    m_colorAttachments[layout]
        .setAttachment(attachment->getIndex())
        .setLayout(vk::ImageLayout::eColorAttachmentOptimal);
    m_subpassDescription.setColorAttachments(m_colorAttachments);
    uses(attachment, Resource::Access::ReadWrite);
}

void GraphicsNode::setDepthStencilAttachment(AttachmentImage* attachment)
{
    m_depthStencilAttachment.setAttachment(attachment->getIndex());
    m_subpassDescription.setPDepthStencilAttachment(&m_depthStencilAttachment);
    uses(attachment, Resource::Access::ReadWrite);
}
