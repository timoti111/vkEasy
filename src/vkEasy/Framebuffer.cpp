#include <vkEasy/Device.h>
#include <vkEasy/Framebuffer.h>
#include <vkEasy/Graph.h>

using namespace VK_EASY_NAMESPACE;

Framebuffer::Framebuffer()
    : Errorable("Framebuffer")
{
}

void Framebuffer::begin(vk::raii::CommandBuffer* commandBuffer)
{
    if (m_runtimeReferences == 0) {
        m_clearValues.clear();
        for (auto& attachment : m_attachments)
            m_clearValues.push_back(attachment->getClearValue());
        m_renderPassBeginInfo.setClearValues(m_clearValues);
        m_renderPassBeginInfo.setFramebuffer(**m_frameBuffers[getGraph()->getImageIndex()]);
        m_renderPassBeginInfo.setRenderArea(m_renderArea);
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
    if (m_needsRecreation) {
        getDevice()->wait();

        std::vector<vk::SubpassDependency> deps;
        uint32_t srcDep = VK_SUBPASS_EXTERNAL;

        m_attachmentViews.clear();
        m_frameBuffers.clear();
        std::vector<vk::SubpassDescription> subpasses;
        uint32_t actualSubpass = 0;
        for (auto& node : m_subpassNodes) {
            subpasses.push_back(node->m_subpassDescription);
            vk::SubpassDependency dep;
            dep.setSrcSubpass(srcDep)
                .setDstSubpass(actualSubpass)
                .setSrcStageMask(vk::PipelineStageFlagBits::eTopOfPipe)
                .setDstStageMask(vk::PipelineStageFlagBits::eBottomOfPipe);
            srcDep = actualSubpass++;
            deps.push_back(dep);
        }

        m_attachmentDescriptions.clear();
        for (auto& attachment : m_attachments) {
            vk::AttachmentDescription description;
            description.setFormat(attachment->getFormat())
                .setSamples(attachment->getSamplesPerTexel())
                .setLoadOp(vk::AttachmentLoadOp::eClear)
                .setStoreOp(vk::AttachmentStoreOp::eStore)
                .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
                .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
                .setInitialLayout(
                    attachment->getRequiredLayout(vk::PipelineStageFlagBits::eAllGraphics, Resource::Access::ReadWrite))
                .setFinalLayout(attachment->getRequiredLayout(
                    vk::PipelineStageFlagBits::eAllGraphics, Resource::Access::ReadWrite));
            m_attachmentDescriptions.push_back(description);
        }

        m_renderPassCreateInfo.setSubpasses(subpasses).setDependencies(deps).setAttachments(m_attachmentDescriptions);
        m_renderPass = std::make_unique<vk::raii::RenderPass>(*getDevice()->getLogicalDevice(), m_renderPassCreateInfo);
        m_renderPassBeginInfo.setRenderPass(**m_renderPass);
        m_needsRecreation = false;
    }

    if (m_wsi) {
        auto resolution = m_wsi->resolution();
        setResolution(resolution.width, resolution.height);
    }

    auto imageIndex = getGraph()->getImageIndex();
    m_attachmentViews[imageIndex].clear();
    for (auto& attachment : m_attachments)
        m_attachmentViews[imageIndex].push_back(**attachment->getVkImageView(imageIndex));
    m_framebufferCreateInfo.setRenderPass(**m_renderPass).setAttachments(m_attachmentViews[imageIndex]).setLayers(1);
    m_frameBuffers[imageIndex]
        = std::make_unique<vk::raii::Framebuffer>(*getDevice()->getLogicalDevice(), m_framebufferCreateInfo);
}

ColorAttachment* Framebuffer::createColorAttachment()
{
    auto attachment = &getGraph()->createResource<ColorAttachment>();
    initializeAttachment(attachment);
    return attachment;
}

DepthStencilBuffer* Framebuffer::getDepthStencilAttachment()
{
    if (!m_depthStencilAttachment) {
        m_depthStencilAttachment = &getGraph()->createResource<DepthStencilBuffer>();
        initializeAttachment(m_depthStencilAttachment);
    }
    return m_depthStencilAttachment;
}

void Framebuffer::setResolution(size_t width, size_t height)
{
    m_framebufferCreateInfo.setWidth(width);
    m_framebufferCreateInfo.setHeight(height);
    m_renderArea.setExtent(vk::Extent2D(width, height));
    for (auto& attachment : m_attachments)
        attachment->setDimensions(vk::Extent3D(width, height, 1));
}

void Framebuffer::setWindow(WSI& window)
{
    m_wsi = &window;
    auto resolution = window.resolution();
    setResolution(resolution.width, resolution.height);
    window.onResolutionChanged([this](const vk::Extent2D& dimensions) {
        m_needsRecreation = true;
        setResolution(dimensions.width, dimensions.height);
        for (auto& node : m_subpassNodes)
            node->needsRebuild();
    });
    initializeAttachment(window.getAttachment());
}

Image* Framebuffer::initializeAttachment(Image* attachment)
{
    attachment->setIndex(m_attachments.size());
    m_attachments.push_back(attachment);
    attachment->setDimensions(vk::Extent3D(m_framebufferCreateInfo.width, m_framebufferCreateInfo.height, 1));
    return attachment;
}
