#pragma once
#include <vkEasy/global.h>
#include <vkEasy/resources/base/Image.h>

namespace VK_EASY_NAMESPACE {
class AttachmentImage : public Image {
    friend class Graph;
    friend class Framebuffer;
    friend class GraphicsNode;

public:
    AttachmentImage(AttachmentImage const&) = delete;
    void operator=(AttachmentImage const&) = delete;

protected:
    AttachmentImage();
    void setIndex(size_t frameBufferIndex);
    size_t getIndex();
    virtual vk::ImageLayout getRequiredLayout(vk::PipelineStageFlagBits stage, Access access);

private:
    size_t m_index;
};
}
