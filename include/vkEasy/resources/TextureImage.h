#pragma once
#include <vkEasy/global.h>
#include <vkEasy/resources/ColorAttachment.h>

namespace VK_EASY_NAMESPACE {
class TextureImage : public ColorAttachment {
    friend class Graph;

public:
    TextureImage(TextureImage const&) = delete;
    void operator=(TextureImage const&) = delete;

protected:
    TextureImage();
    virtual vk::ImageLayout getRequiredLayout(vk::PipelineStageFlagBits stage, Access access);
};
}
