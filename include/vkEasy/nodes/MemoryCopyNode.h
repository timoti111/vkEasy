#pragma once
#include <vkEasy/nodes/base/Node.h>
#include <vkEasy/resources/base/Buffer.h>
#include <vkEasy/resources/base/Image.h>

namespace VK_EASY_NAMESPACE {
class MemoryCopyNode : public Node {
    friend class Graph;

public:
    MemoryCopyNode(MemoryCopyNode const&) = delete;
    void operator=(MemoryCopyNode const&) = delete;

    void setSrcBuffer(
        Buffer& buffer, size_t size = VK_WHOLE_SIZE, size_t offset = 0, vk::Extent2D imageDimensions = vk::Extent2D());
    void setDstBuffer(Buffer& buffer, size_t offset = 0, vk::Extent2D imageDimensions = vk::Extent2D());
    void setSrcImage(Image& image, vk::Extent3D dimensions = vk::Extent3D(VK_WHOLE_SIZE, VK_WHOLE_SIZE, VK_WHOLE_SIZE),
        vk::Offset3D offset = vk::Offset3D(), uint32_t mipLevel = 0, uint32_t baseArrayLayer = 0,
        uint32_t layerCount = 1);
    void setDstImage(Image& image, vk::Offset3D offset = vk::Offset3D(), uint32_t mipLevel = 0,
        uint32_t baseArrayLayer = 0, uint32_t layerCount = 1);

protected:
    MemoryCopyNode();
    void update();

private:
    vk::BufferCopy m_bufferCopyRegion;
    vk::BufferImageCopy m_bufferImageCopyRegion;
    vk::ImageCopy m_imageCopyRegion;
    vk::ImageSubresourceLayers m_srcSubresource;
    Image* m_srcImage = nullptr;
    Buffer* m_srcBuffer = nullptr;
    vk::ImageSubresourceLayers m_dstSubresource;
    Image* m_dstImage = nullptr;
    Buffer* m_dstBuffer = nullptr;
};
} // namespace VK_EASY_NAMESPACE