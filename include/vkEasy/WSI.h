#pragma once

// #include <vkEasy/Device.h>
#include <vkEasy/Error.h>
#include <vkEasy/SwapChain.h>
#include <vkEasy/global.h>

namespace VK_EASY_NAMESPACE {
class Device;

class WSI : public Errorable {
    friend class Device;
    friend class SwapChain;

public:
    WSI(WSI const&) = delete;
    void operator=(WSI const&) = delete;
    virtual ~WSI() = default;

    virtual vk::Extent2D resolution() = 0;

protected:
    explicit WSI(uint32_t width, uint32_t height, const std::string& title, Device* parent);

    vk::easy::SwapChain m_swapChain;
    vk::raii::SurfaceKHR m_surface = nullptr;
    std::string m_name;
    vk::Extent2D m_extent;
    Device* m_parent;
};
} // namespace VK_EASY_NAMESPACE
