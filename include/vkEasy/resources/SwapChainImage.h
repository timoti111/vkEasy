#pragma once
#include <vkEasy/global.h>
#include <vkEasy/resources/ColorAttachment.h>

namespace VK_EASY_NAMESPACE {
class Graph;
class WSI;

class SwapChainImage : public ColorAttachment {
    friend class Graph;
    friend class WSI;
    friend class Framebuffer;

public:
    SwapChainImage(SwapChainImage const&) = delete;
    void operator=(SwapChainImage const&) = delete;

protected:
    SwapChainImage();

    virtual void update();
    virtual bool exists();
    void recreate();
    uint32_t getNumberOfSwapchainFrames();

private:
    void prepareForPresentation();
    WSI* m_window;
    bool m_recreate = false;
    std::unique_ptr<vk::raii::SwapchainKHR> m_swapChain;
    vk::SwapchainCreateInfoKHR m_swapChainCreateInfo;
    vk::SurfaceCapabilitiesKHR m_surfaceCapabilities;
};
}
