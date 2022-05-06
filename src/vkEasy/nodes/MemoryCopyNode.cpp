#include <iostream>
#include <vkEasy/Device.h>
#include <vkEasy/nodes/MemoryCopyNode.h>

using namespace VK_EASY_NAMESPACE;

MemoryCopyNode::MemoryCopyNode()
    : Node("BufferCopyNode")
{
    m_neededQueueType = vk::QueueFlagBits::eTransfer;
    m_pipelineStage = vk::PipelineStageFlagBits::eTransfer;
}

void MemoryCopyNode::update()
{
    auto transferBuffers = getCommandBuffers(1);
    if (transferBuffers.empty())
        return;

    if (m_srcBuffer && m_dstBuffer) {
        if (m_bufferCopyRegion.size == VK_WHOLE_SIZE)
            m_bufferCopyRegion.setSize(m_srcBuffer->getSize() - m_bufferCopyRegion.srcOffset);
        transferBuffers[0]->copyBuffer(m_srcBuffer->getVkBuffer(), m_dstBuffer->getVkBuffer(), m_bufferCopyRegion);
    } else if (m_srcImage && m_dstBuffer) {
        if (m_bufferImageCopyRegion.imageExtent == vk::Extent3D(VK_WHOLE_SIZE, VK_WHOLE_SIZE, VK_WHOLE_SIZE))
            m_bufferImageCopyRegion.setImageExtent(m_srcImage->getDimensions());
        transferBuffers[0]->copyImageToBuffer(m_srcImage->getVkImage(), vk::ImageLayout::eTransferSrcOptimal,
            m_dstBuffer->getVkBuffer(), m_bufferImageCopyRegion);
    } else if (m_srcBuffer && m_dstImage) {
        if (m_bufferImageCopyRegion.imageExtent == vk::Extent3D(VK_WHOLE_SIZE, VK_WHOLE_SIZE, VK_WHOLE_SIZE)
            || m_bufferImageCopyRegion.imageExtent == vk::Extent3D())
            m_bufferImageCopyRegion.setImageExtent(m_dstImage->getDimensions());
        transferBuffers[0]->copyBufferToImage(m_srcBuffer->getVkBuffer(), m_dstImage->getVkImage(),
            vk::ImageLayout::eTransferDstOptimal, m_bufferImageCopyRegion);
    } else if (m_srcImage && m_dstImage) {
        if (m_imageCopyRegion.extent == vk::Extent3D(VK_WHOLE_SIZE, VK_WHOLE_SIZE, VK_WHOLE_SIZE))
            m_imageCopyRegion.setExtent(m_srcImage->getDimensions());
        transferBuffers[0]->copyImage(m_srcImage->getVkImage(), vk::ImageLayout::eTransferSrcOptimal,
            m_dstImage->getVkImage(), vk::ImageLayout::eTransferDstOptimal, m_imageCopyRegion);
    }
}

void MemoryCopyNode::setSrcBuffer(Buffer& buffer, size_t size, size_t offset, vk::Extent2D imageDimensions)
{
    uses(&buffer, Resource::Access::ReadOnly);
    m_srcBuffer = &buffer;
    m_srcImage = nullptr;
    m_bufferImageCopyRegion.setBufferOffset(offset)
        .setBufferRowLength(imageDimensions.width)
        .setBufferImageHeight(imageDimensions.height);
    m_bufferCopyRegion.setSrcOffset(offset).setSize(size);
}

void MemoryCopyNode::setDstBuffer(Buffer& buffer, size_t offset, vk::Extent2D imageDimensions)
{
    uses(&buffer, Resource::Access::ReadWrite);
    m_dstBuffer = &buffer;
    m_dstImage = nullptr;
    m_bufferImageCopyRegion.setBufferOffset(offset)
        .setBufferRowLength(imageDimensions.width)
        .setBufferImageHeight(imageDimensions.height);
    m_bufferCopyRegion.setSrcOffset(offset);
}

void MemoryCopyNode::setSrcImage(Image& image, vk::Extent3D dimensions, vk::Offset3D offset, uint32_t mipLevel,
    uint32_t baseArrayLayer, uint32_t layerCount)
{
    uses(&image, Resource::Access::ReadOnly);
    m_srcImage = &image;
    m_srcBuffer = nullptr;
    m_srcSubresource.setAspectMask(image.getAspectMask())
        .setBaseArrayLayer(baseArrayLayer)
        .setLayerCount(layerCount)
        .setMipLevel(mipLevel);
    m_bufferImageCopyRegion.setImageExtent(dimensions).setImageOffset(offset).setImageSubresource(m_srcSubresource);
    m_imageCopyRegion.setSrcOffset(offset).setExtent(dimensions).setSrcSubresource(m_srcSubresource);
}

void MemoryCopyNode::setDstImage(
    Image& image, vk::Offset3D offset, uint32_t mipLevel, uint32_t baseArrayLayer, uint32_t layerCount)
{
    uses(&image, Resource::Access::ReadWrite);
    m_dstImage = &image;
    m_dstBuffer = nullptr;
    m_dstSubresource.setAspectMask(image.getAspectMask())
        .setBaseArrayLayer(baseArrayLayer)
        .setLayerCount(layerCount)
        .setMipLevel(mipLevel);
    m_bufferImageCopyRegion.setImageOffset(offset).setImageSubresource(m_dstSubresource);
    m_imageCopyRegion.setDstOffset(offset).setDstSubresource(m_dstSubresource);
}
