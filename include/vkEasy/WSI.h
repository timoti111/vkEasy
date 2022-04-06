#pragma once

#include <vkEasy/Error.h>
#include <vkEasy/Utils.h>
#include <vkEasy/global.h>
#include <vkEasy/resources/SwapChainImage.h>

namespace VK_EASY_NAMESPACE {
class WSI : public Errorable, public Object {
    friend class Graph;
    friend class Device;
    friend class SwapChainImage;

public:
    WSI(WSI const&) = delete;
    void operator=(WSI const&) = delete;
    virtual ~WSI() = default;

    virtual bool shouldClose() = 0;
    virtual void pollEvents() = 0;
    vk::Extent2D resolution();
    AttachmentImage* getAttachment();
    void onResolutionChanged(std::function<void(const vk::Extent2D&)> event);

protected:
    explicit WSI(uint32_t width, uint32_t height, const std::string& title, Graph* parent);
    virtual void createSurface() = 0;
    void update();
    void setSwapchainResolution(const vk::Extent2D& resoution);
    virtual vk::Extent2D osWindowResolution() = 0;
    void recreateSwapchain();

    std::unique_ptr<vk::raii::SurfaceKHR> m_surface = nullptr;
    std::string m_name;
    vk::Extent2D m_extent;
    vk::easy::SwapChainImage* m_swapChain;
    std::vector<std::function<void(vk::Extent2D&)>> m_resolutionChangedEvents;
};
} // namespace VK_EASY_NAMESPACE
