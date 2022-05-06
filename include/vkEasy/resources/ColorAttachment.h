#pragma once
#include <vkEasy/global.h>
#include <vkEasy/resources/base/Image.h>

namespace VK_EASY_NAMESPACE {
class ColorAttachment : public Image {
    friend class Graph;

public:
    ColorAttachment(ColorAttachment const&) = delete;
    void operator=(ColorAttachment const&) = delete;

    void setClearColor(vk::ClearColorValue color);

protected:
    ColorAttachment();
    virtual vk::ImageLayout getRequiredLayout(vk::PipelineStageFlagBits stage, Access access);
    virtual vk::ClearValue getClearValue();
    vk::ClearColorValue m_clearValue;
};
}
