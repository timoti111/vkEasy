#pragma once
#include <vkEasy/Error.h>
#include <vkEasy/Utils.h>
#include <vkEasy/WSI.h>
#include <vkEasy/global.h>
#include <vkEasy/resources/AttachmentImage.h>

namespace VK_EASY_NAMESPACE {
class GraphicsNode;
class Framebuffer : public Errorable, public Object {
    friend class Graph;
    friend class GraphicsNode;

public:
    Framebuffer(Framebuffer const&) = delete;
    void operator=(Framebuffer const&) = delete;

    AttachmentImage* createAttachment();
    AttachmentImage* getDepthStencilAttachment();

    void setWindow(WSI& window);
    void setResolution(size_t width, size_t height);

private:
    Framebuffer();
    void build();
    void begin(vk::raii::CommandBuffer* commandBuffer);
    void end(vk::raii::CommandBuffer* commandBuffer);
    AttachmentImage* initializeAttachment(AttachmentImage* attachment);

    std::vector<vk::AttachmentDescription> m_attachmentDescriptions;
    std::vector<AttachmentImage*> m_attachments;
    AttachmentImage* m_depthStencilAttachment = nullptr;

    vk::RenderPassBeginInfo m_renderPassBeginInfo;

    std::vector<vk::SubpassDescription*> m_subpasses;
    vk::RenderPassCreateInfo m_renderPassCreateInfo;
    std::unique_ptr<vk::raii::RenderPass> m_renderPass;

    std::vector<std::vector<vk::ImageView>> m_attachmentViews;
    std::vector<std::unique_ptr<vk::raii::Framebuffer>> m_frameBuffers;
    vk::FramebufferCreateInfo m_framebufferCreateInfo;

    WSI* m_wsi = nullptr;
    size_t m_references = 0;
    size_t m_runtimeReferences = 0;
    vk::Rect2D m_renderArea;
};
} // namespace VK_EASY_NAMESPACE
