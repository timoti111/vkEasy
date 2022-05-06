#pragma once
#include <vkEasy/Error.h>
#include <vkEasy/Utils.h>
#include <vkEasy/WSI.h>
#include <vkEasy/global.h>
#include <vkEasy/resources/ColorAttachment.h>
#include <vkEasy/resources/DepthStencilBuffer.h>

namespace VK_EASY_NAMESPACE {
class GraphicsNode;
class Framebuffer : public Errorable, public Object {
    friend class Graph;
    friend class GraphicsNode;

public:
    Framebuffer(Framebuffer const&) = delete;
    void operator=(Framebuffer const&) = delete;

    ColorAttachment* createColorAttachment();
    DepthStencilBuffer* getDepthStencilAttachment();

    void setWindow(WSI& window);
    void setResolution(size_t width, size_t height);

private:
    Framebuffer();
    void build();
    void begin(vk::raii::CommandBuffer* commandBuffer);
    void end(vk::raii::CommandBuffer* commandBuffer);
    Image* initializeAttachment(Image* attachment);

    std::vector<vk::AttachmentDescription> m_attachmentDescriptions;
    std::vector<Image*> m_attachments;
    DepthStencilBuffer* m_depthStencilAttachment = nullptr;

    vk::RenderPassBeginInfo m_renderPassBeginInfo;

    vk::RenderPassCreateInfo m_renderPassCreateInfo;
    std::unique_ptr<vk::raii::RenderPass> m_renderPass;

    std::map<size_t, std::vector<vk::ImageView>> m_attachmentViews;
    std::map<size_t, std::unique_ptr<vk::raii::Framebuffer>> m_frameBuffers;
    vk::FramebufferCreateInfo m_framebufferCreateInfo;
    std::vector<GraphicsNode*> m_subpassNodes;

    WSI* m_wsi = nullptr;
    size_t m_references = 0;
    size_t m_runtimeReferences = 0;
    vk::Rect2D m_renderArea;
    bool m_needsRecreation = false;

    std::vector<vk::ClearValue> m_clearValues;
};
} // namespace VK_EASY_NAMESPACE
