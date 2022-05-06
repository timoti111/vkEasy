#pragma once
#include <vkEasy/global.h>
#include <vkEasy/resources/base/Image.h>

namespace VK_EASY_NAMESPACE {
class DepthStencilBuffer : public Image {
    friend class Graph;

public:
    DepthStencilBuffer(DepthStencilBuffer const&) = delete;
    void operator=(DepthStencilBuffer const&) = delete;

    void setClearValue(vk::ClearDepthStencilValue value);

protected:
    DepthStencilBuffer();
    virtual vk::ImageLayout getRequiredLayout(vk::PipelineStageFlagBits stage, Access access);
    virtual vk::ClearValue getClearValue();
    vk::ClearDepthStencilValue m_clearValue;
};
}
