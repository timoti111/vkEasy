#include <vkEasy/Device.h>
#include <vkEasy/Framebuffer.h>
#include <vkEasy/Graph.h>

using namespace VK_EASY_NAMESPACE;

Framebuffer::Framebuffer()
    : Errorable("Framebuffer")
{
    vk::ClearColorValue color;
    color.setFloat32({ 0.0f, 0.0f, 0.0f, 1.0f });
    vk::ClearValue value;
    value.setColor(color);
    m_renderPassBeginInfo.setClearValues(value);
}

void Framebuffer::begin(vk::raii::CommandBuffer* commandBuffer)
{
    if (m_runtimeReferences == 0) {
        m_renderPassBeginInfo.setFramebuffer(**m_frameBuffers[getGraph()->getImageIndex()]);
        commandBuffer->beginRenderPass(m_renderPassBeginInfo, vk::SubpassContents::eInline);
    }
    m_runtimeReferences++;
}

void Framebuffer::end(vk::raii::CommandBuffer* commandBuffer)
{
    if (m_runtimeReferences != m_references)
        commandBuffer->nextSubpass(vk::SubpassContents::eInline);
    else {
        commandBuffer->endRenderPass();
        m_runtimeReferences = 0;
    }
}

void Framebuffer::build()
{
    for (auto& attachment : m_attachments) {
        vk::AttachmentDescription description;
        description.setFormat(attachment->getFormat())
            .setSamples(attachment->getSamplesPerTexel())
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
        m_attachmentDescriptions.push_back(description);
    }

    std::vector<vk::SubpassDescription> subpasses;
    for (auto& subpass : m_subpasses)
        subpasses.push_back(*subpass);
    vk::SubpassDependency dep;
    dep.setSrcSubpass(VK_SUBPASS_EXTERNAL)
        .setDstSubpass(0)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
        .setSrcAccessMask(vk::AccessFlagBits::eNone)
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
    m_renderPassCreateInfo.setSubpasses(subpasses).setDependencies(dep).setAttachments(m_attachmentDescriptions);
    m_renderPass = std::make_unique<vk::raii::RenderPass>(*getDevice()->getLogicalDevice(), m_renderPassCreateInfo);

    if (m_wsi) {
        auto resolution = m_wsi->resolution();
        m_framebufferCreateInfo.setWidth(resolution.width).setHeight(resolution.height);
    }

    m_attachmentViews.resize(getGraph()->getFrames());
    for (size_t i = 0; i < getGraph()->getFrames(); i++) {
        for (auto& attachment : m_attachments)
            m_attachmentViews[i].push_back(**attachment->getVkImageView(i));

        m_framebufferCreateInfo.setRenderPass(**m_renderPass).setAttachments(m_attachmentViews[i]).setLayers(1);
        m_frameBuffers.push_back(
            std::make_unique<vk::raii::Framebuffer>(*getDevice()->getLogicalDevice(), m_framebufferCreateInfo));
    }

    m_renderPassBeginInfo.setRenderPass(**m_renderPass);
}

AttachmentImage* Framebuffer::createAttachment()
{
    return initializeAttachment(&getGraph()->createResource<AttachmentImage>());
}

AttachmentImage* Framebuffer::getDepthStencilAttachment()
{
    if (!m_depthStencilAttachment)
        m_depthStencilAttachment = initializeAttachment(&getGraph()->createResource<AttachmentImage>());
    return m_depthStencilAttachment;
}

void Framebuffer::setResolution(size_t width, size_t height)
{
    m_framebufferCreateInfo.setWidth(width);
    m_framebufferCreateInfo.setHeight(height);
}

void Framebuffer::setWindow(WSI& window)
{
    m_wsi = &window;
    initializeAttachment(window.getAttachment());
}

AttachmentImage* Framebuffer::initializeAttachment(AttachmentImage* attachment)
{
    attachment->setIndex(m_attachments.size());
    m_attachments.push_back(attachment);
    return attachment;
}
