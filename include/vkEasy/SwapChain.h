#pragma once
#include <vkEasy/Error.h>
#include <vkEasy/global.h>

namespace VK_EASY_NAMESPACE {
class Device;
class WSI;

class SwapChain : public Errorable {
    friend class Device;
    friend class WSI;

public:
    SwapChain(SwapChain const&) = delete;
    void operator=(SwapChain const&) = delete;

private:
    explicit SwapChain(WSI* parent);
    void update();

    typedef struct _SwapChainBuffers {
        _SwapChainBuffers(VkImage image, vk::raii::Device& device, SwapChain* swapChain);
        std::unique_ptr<vk::raii::Image> image;
        std::unique_ptr<vk::raii::ImageView> view;
        std::unique_ptr<vk::raii::Semaphore> imageAvailableSemaphore;
        std::unique_ptr<vk::raii::Semaphore> renderFinishedSemaphore;
        std::unique_ptr<vk::raii::Fence> inFlightFence;
    } SwapChainBuffer;

    WSI* m_parent;
    std::unique_ptr<vk::raii::SwapchainKHR> m_swapChain;
    vk::SwapchainCreateInfoKHR m_swapChainCreateInfo;
    vk::SurfaceCapabilitiesKHR m_surfaceCapabilities;

    std::vector<SwapChainBuffer> m_buffers;
};
} // namespace VK_EASY_NAMESPACE
