#pragma once
#include <vkEasy/global.h>
#include <vkEasy/resources/AttachmentImage.h>

namespace VK_EASY_NAMESPACE {
class Graph;
class WSI;

class SwapChainImage : public AttachmentImage {
    friend class Graph;
    friend class WSI;
    friend class Framebuffer;

public:
    SwapChainImage(SwapChainImage const&) = delete;
    void operator=(SwapChainImage const&) = delete;
    VkImage getVkImage();
    virtual vk::raii::ImageView* getVkImageView(uint32_t imageIndex) override;

protected:
    SwapChainImage();

    void destroy();
    virtual void update();
    virtual bool exists();
    uint32_t getNumberOfFramesInFlight();

private:
    std::vector<VkImage> m_images;
    std::vector<std::unique_ptr<vk::raii::ImageView>> m_views;

    WSI* m_window;
    std::unique_ptr<vk::raii::SwapchainKHR> m_swapChain;
    vk::SwapchainCreateInfoKHR m_swapChainCreateInfo;
    vk::SurfaceCapabilitiesKHR m_surfaceCapabilities;
};
}
