#include <vkEasy/Graph.h>
#include <vkEasy/WSI.h>

using namespace VK_EASY_NAMESPACE;

WSI::WSI(uint32_t width, uint32_t height, const std::string& title, Graph* parent)
    : Errorable("WSI")
    , m_name(title)
    , m_extent(width, height)
{
    m_swapChain = &parent->createResource<SwapChainImage>();
    m_swapChain->m_window = this;
}

void WSI::update()
{
    if (!m_surface)
        createSurface();
}

ColorAttachment* WSI::getAttachment()
{
    return m_swapChain;
}

void WSI::onResolutionChanged(std::function<void(const vk::Extent2D&)> event)
{
    m_resolutionChangedEvents.push_back(event);
}

vk::Extent2D WSI::resolution()
{
    return m_extent;
}

void WSI::setSwapchainResolution(const vk::Extent2D& resoution)
{
    m_extent = resoution;
    for (auto& f : m_resolutionChangedEvents)
        f(m_extent);
}

void WSI::recreateSwapchain()
{
    m_swapChain->recreate();
}
