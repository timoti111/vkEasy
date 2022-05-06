#include <vkEasy/Device.h>
#include <vkEasy/nodes/GraphicsNode.h>

using namespace VK_EASY_NAMESPACE;

GraphicsNode::GraphicsNode()
    : PipelineNode("GraphicsNode")
{
    // m_dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eLineWidth };
    m_neededQueueType = vk::QueueFlagBits::eGraphics;
    m_pipelineStage = vk::PipelineStageFlagBits::eAllGraphics;

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
    m_framebuffer->build();
    m_basePipelineUpdateFunction();

    auto graphicsBuffers = getCommandBuffers(1);
    if (graphicsBuffers.empty())
        return;

    m_framebuffer->begin(graphicsBuffers[0]);
    graphicsBuffers[0]->bindPipeline(vk::PipelineBindPoint::eGraphics, **m_pipeline);
    if (!m_descriptorSetsToBind.empty())
        graphicsBuffers[0]->bindDescriptorSets(vk::PipelineBindPoint::eGraphics, **m_pipelineLayout, 0,
            m_descriptorSetsToBind[getGraph()->getImageIndex()], {});
    if (!m_vertexBuffers.empty())
        graphicsBuffers[0]->bindVertexBuffers(0, m_vertexBuffers, m_vertexBufferOffsets);

    if (m_indexBuffer) {
        graphicsBuffers[0]->bindIndexBuffer(vk::Buffer(m_indexBuffer->getVkBuffer()), 0, m_indexBuffer->m_indexType);
        graphicsBuffers[0]->drawIndexed(m_indexBuffer->m_numberOfIndices, m_numberOfInstances, 0, 0, 0);
    } else
        graphicsBuffers[0]->draw(m_numberOfVertices, m_numberOfInstances, 0, 0);
    m_framebuffer->end(graphicsBuffers[0]);
}

void GraphicsNode::buildPipeline()
{
    std::vector<vk::PipelineShaderStageCreateInfo> stageInfos;
    for (auto& [key, element] : m_stages)
        stageInfos.push_back(*element->getPipelineShaderStageCreateInfo());

    if (!m_vertexBufferInfos.empty()) {
        m_vertexBuffers.clear();
        m_vertexBufferOffsets.clear();
        for (auto& bufferInfo : m_vertexBufferInfos) {
            m_vertexBuffers.push_back(bufferInfo.buffer->getVkBuffer());
            m_vertexBufferOffsets.push_back(bufferInfo.offsetInBuffer);
        }
        // m_bindingDescription.setBinding(0);
        // m_bindingDescription.setInputRate(
        //     m_numberOfInstances == 1 ? vk::VertexInputRate::eVertex : vk::VertexInputRate::eInstance);
        // m_bindingDescription.setStride(m_vertexBuffer->getStride());
        m_vertexInputState.setVertexAttributeDescriptions(m_attributes);
        m_vertexInputState.setVertexBindingDescriptions(m_bindingDescriptions);
    }

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

Descriptor* GraphicsNode::setInputAttachment(Image* attachment, size_t attachmentIndex, size_t binding, size_t set)
{
    if (attachmentIndex + 1 > m_inputAttachments.size())
        m_inputAttachments.resize(attachmentIndex + 1);
    attachment->addImageUsageFlag(vk::ImageUsageFlagBits::eInputAttachment);
    m_inputAttachments[attachmentIndex]
        .setAttachment(attachment->getIndex())
        .setLayout(attachment->getRequiredLayout(vk::PipelineStageFlagBits::eAllGraphics, Resource::Access::ReadWrite));
    m_subpassDescription.setInputAttachments(m_inputAttachments);
    return createDescriptor({ attachment }, binding, set);
}

void GraphicsNode::setColorAttachment(ColorAttachment* attachment, size_t layout)
{
    Image* image = attachment;
    if (layout + 1 > m_colorAttachments.size())
        m_colorAttachments.resize(layout + 1);
    attachment->addImageUsageFlag(vk::ImageUsageFlagBits::eColorAttachment);
    m_colorAttachments[layout]
        .setAttachment(attachment->getIndex())
        .setLayout(image->getRequiredLayout(vk::PipelineStageFlagBits::eAllGraphics, Resource::Access::ReadWrite));
    m_subpassDescription.setColorAttachments(m_colorAttachments);
    uses(attachment, Resource::Access::ReadWrite);
}

void GraphicsNode::setDepthStencilAttachment(DepthStencilBuffer* attachment)
{
    Image* image = attachment;
    m_depthStencilState.setDepthTestEnable(true)
        .setDepthWriteEnable(true)
        .setDepthCompareOp(vk::CompareOp::eLess)
        .setMaxDepthBounds(1.0f);
    attachment->addImageUsageFlag(vk::ImageUsageFlagBits::eDepthStencilAttachment);
    m_depthStencilAttachment.setAttachment(attachment->getIndex())
        .setLayout(image->getRequiredLayout(vk::PipelineStageFlagBits::eAllGraphics, Resource::Access::ReadWrite));
    m_subpassDescription.setPDepthStencilAttachment(&m_depthStencilAttachment);
    uses(attachment, Resource::Access::ReadWrite);
}

void GraphicsNode::setIndexBuffer(IndexBuffer* attachment)
{
    m_indexBuffer = attachment;
    uses(attachment, Resource::Access::ReadOnly);
}

void GraphicsNode::setNumberOfInstances(uint32_t instances)
{
    m_numberOfInstances = instances;
}

void GraphicsNode::setNumberOfVertices(uint32_t vertices)
{
    m_numberOfVertices = vertices;
}

void GraphicsNode::setTopology(vk::PrimitiveTopology topology)
{
    m_inputAssemblyState.setTopology(topology);
}

template <>
void GraphicsNode::defineAttribute<int>(
    uint32_t location, uint32_t offset, uint32_t stride, VertexBuffer* buffer, vk::DeviceSize offsetInBuffer)
{
    defineAttribute(location, offset, vk::Format::eR32Sint, stride, buffer, offsetInBuffer);
}

template <>
void GraphicsNode::defineAttribute<unsigned>(
    uint32_t location, uint32_t offset, uint32_t stride, VertexBuffer* buffer, vk::DeviceSize offsetInBuffer)
{
    defineAttribute(location, offset, vk::Format::eR32Uint, stride, buffer, offsetInBuffer);
}

template <>
void GraphicsNode::defineAttribute<float>(
    uint32_t location, uint32_t offset, uint32_t stride, VertexBuffer* buffer, vk::DeviceSize offsetInBuffer)
{
    defineAttribute(location, offset, vk::Format::eR32Sfloat, stride, buffer, offsetInBuffer);
}

template <>
void GraphicsNode::defineAttribute<double>(
    uint32_t location, uint32_t offset, uint32_t stride, VertexBuffer* buffer, vk::DeviceSize offsetInBuffer)
{
    defineAttribute(location, offset, vk::Format::eR64Sfloat, stride, buffer, offsetInBuffer);
}

template <>
void GraphicsNode::defineAttribute<glm::vec1>(
    uint32_t location, uint32_t offset, uint32_t stride, VertexBuffer* buffer, vk::DeviceSize offsetInBuffer)
{
    defineAttribute(location, offset, vk::Format::eR32Sfloat, stride, buffer, offsetInBuffer);
}

template <>
void GraphicsNode::defineAttribute<glm::vec2>(
    uint32_t location, uint32_t offset, uint32_t stride, VertexBuffer* buffer, vk::DeviceSize offsetInBuffer)
{
    defineAttribute(location, offset, vk::Format::eR32G32Sfloat, stride, buffer, offsetInBuffer);
}

template <>
void GraphicsNode::defineAttribute<glm::vec3>(
    uint32_t location, uint32_t offset, uint32_t stride, VertexBuffer* buffer, vk::DeviceSize offsetInBuffer)
{
    defineAttribute(location, offset, vk::Format::eR32G32B32Sfloat, stride, buffer, offsetInBuffer);
}

template <>
void GraphicsNode::defineAttribute<glm::vec4>(
    uint32_t location, uint32_t offset, uint32_t stride, VertexBuffer* buffer, vk::DeviceSize offsetInBuffer)
{
    defineAttribute(location, offset, vk::Format::eR32G32B32A32Sfloat, stride, buffer, offsetInBuffer);
}

template <>
void GraphicsNode::defineAttribute<glm::uvec1>(
    uint32_t location, uint32_t offset, uint32_t stride, VertexBuffer* buffer, vk::DeviceSize offsetInBuffer)
{
    defineAttribute(location, offset, vk::Format::eR32Uint, stride, buffer, offsetInBuffer);
}

template <>
void GraphicsNode::defineAttribute<glm::uvec2>(
    uint32_t location, uint32_t offset, uint32_t stride, VertexBuffer* buffer, vk::DeviceSize offsetInBuffer)
{
    defineAttribute(location, offset, vk::Format::eR32G32Uint, stride, buffer, offsetInBuffer);
}

template <>
void GraphicsNode::defineAttribute<glm::uvec3>(
    uint32_t location, uint32_t offset, uint32_t stride, VertexBuffer* buffer, vk::DeviceSize offsetInBuffer)
{
    defineAttribute(location, offset, vk::Format::eR32G32B32Uint, stride, buffer, offsetInBuffer);
}

template <>
void GraphicsNode::defineAttribute<glm::uvec4>(
    uint32_t location, uint32_t offset, uint32_t stride, VertexBuffer* buffer, vk::DeviceSize offsetInBuffer)
{
    defineAttribute(location, offset, vk::Format::eR32G32B32A32Uint, stride, buffer, offsetInBuffer);
}

template <>
void GraphicsNode::defineAttribute<glm::ivec1>(
    uint32_t location, uint32_t offset, uint32_t stride, VertexBuffer* buffer, vk::DeviceSize offsetInBuffer)
{
    defineAttribute(location, offset, vk::Format::eR32Sint, stride, buffer, offsetInBuffer);
}

template <>
void GraphicsNode::defineAttribute<glm::ivec2>(
    uint32_t location, uint32_t offset, uint32_t stride, VertexBuffer* buffer, vk::DeviceSize offsetInBuffer)
{
    defineAttribute(location, offset, vk::Format::eR32G32Sint, stride, buffer, offsetInBuffer);
}

template <>
void GraphicsNode::defineAttribute<glm::ivec3>(
    uint32_t location, uint32_t offset, uint32_t stride, VertexBuffer* buffer, vk::DeviceSize offsetInBuffer)
{
    defineAttribute(location, offset, vk::Format::eR32G32B32Sint, stride, buffer, offsetInBuffer);
}

template <>
void GraphicsNode::defineAttribute<glm::ivec4>(
    uint32_t location, uint32_t offset, uint32_t stride, VertexBuffer* buffer, vk::DeviceSize offsetInBuffer)
{
    defineAttribute(location, offset, vk::Format::eR32G32B32A32Sint, stride, buffer, offsetInBuffer);
}

void GraphicsNode::defineAttribute(uint32_t location, uint32_t offset, vk::Format format, uint32_t stride,
    VertexBuffer* buffer, vk::DeviceSize offsetInBuffer)
{
    uses(buffer, Resource::Access::ReadOnly);
    vk::VertexInputBindingDescription bindingDescription;
    bindingDescription.setBinding(m_vertexBufferInfos.size()).setStride(stride);
    m_bindingDescriptions.push_back(bindingDescription);
    vk::VertexInputAttributeDescription attribute;
    attribute.setLocation(location).setBinding(m_vertexBufferInfos.size()).setFormat(format).setOffset(offset);
    m_attributes.push_back(attribute);
    m_vertexBufferInfos.push_back({ buffer, offsetInBuffer });
}
