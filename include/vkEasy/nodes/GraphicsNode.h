#pragma once
#include <vkEasy/Framebuffer.h>
#include <vkEasy/nodes/base/PipelineNode.h>
#include <vkEasy/resources/IndexBuffer.h>
#include <vkEasy/resources/VertexBuffer.h>

namespace VK_EASY_NAMESPACE {
class GraphicsNode : public PipelineNode {
    friend class Graph;
    friend class Framebuffer;

public:
    GraphicsNode(GraphicsNode const&) = delete;
    void operator=(GraphicsNode const&) = delete;

    ShaderStage& getVertexShaderStage();
    ShaderStage& getFragmentShaderStage();
    void setFramebuffer(Framebuffer& framebuffer);
    Descriptor* setInputAttachment(Image* attachment, size_t attachmentIndex, size_t binding, size_t set);
    void setColorAttachment(ColorAttachment* attachment, size_t layout);
    void setDepthStencilAttachment(DepthStencilBuffer* attachment);

    void defineAttribute(uint32_t location, uint32_t offset, vk::Format format, uint32_t stride, VertexBuffer* buffer,
        vk::DeviceSize offsetInBuffer = 0);
    template <typename Attribute>
    void defineAttribute(
        uint32_t location, uint32_t offset, uint32_t stride, VertexBuffer* buffer, vk::DeviceSize offsetInBuffer = 0);
    void setIndexBuffer(IndexBuffer* attachment);
    void setNumberOfInstances(uint32_t instances);
    void setNumberOfVertices(uint32_t vertices);
    void setTopology(vk::PrimitiveTopology topology);

protected:
    GraphicsNode();
    void update();
    void buildPipeline();
    void inOrder();

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

    Framebuffer* m_framebuffer = nullptr;
    struct AttributeBuffer {
        VertexBuffer* buffer;
        vk::DeviceSize offsetInBuffer;
    };
    std::vector<AttributeBuffer> m_vertexBufferInfos;
    std::vector<vk::Buffer> m_vertexBuffers;
    std::vector<vk::DeviceSize> m_vertexBufferOffsets;
    std::vector<vk::VertexInputBindingDescription> m_bindingDescriptions;
    std::vector<vk::VertexInputAttributeDescription> m_attributes;
    IndexBuffer* m_indexBuffer = nullptr;
    uint32_t m_numberOfInstances = 1;
    uint32_t m_numberOfVertices = 0;

    vk::AttachmentReference m_depthStencilAttachment;
    std::vector<vk::AttachmentReference> m_colorAttachments;
    std::vector<vk::AttachmentReference> m_inputAttachments;
    vk::SubpassDescription m_subpassDescription;
    size_t m_subpassIndex;

    vk::GraphicsPipelineCreateInfo m_graphicsPipelineCreateInfo;
};
} // namespace VK_EASY_NAMESPACE