#include <vkEasy/Device.h>
#include <vkEasy/WSI.h>
#include <vkEasy/resources/SwapChainImage.h>

using namespace VK_EASY_NAMESPACE;

SwapChainImage::SwapChainImage()
    : AttachmentImage()
{
    m_swapChainCreateInfo.setImageFormat(vk::Format::eB8G8R8A8Srgb);
    m_swapChainCreateInfo.setImageColorSpace(vk::ColorSpaceKHR::eSrgbNonlinear);
    setPersistence(true);
}

void SwapChainImage::update()
{
    if (m_swapChain && !m_recreate)
        return;

    getDevice()->wait();

    m_swapChain.reset();
    m_views.clear();
    vk::raii::PhysicalDevice* device = getDevice()->getPhysicalDevice();
    auto capabilities = device->getSurfaceCapabilitiesKHR(**m_window->m_surface);
    auto formats = device->getSurfaceFormatsKHR(**m_window->m_surface);
    auto presentModes = device->getSurfacePresentModesKHR(**m_window->m_surface);
    bool found = false;
    for (const auto& availableFormat : formats) {
        if (availableFormat.format == m_swapChainCreateInfo.imageFormat
            && availableFormat.colorSpace == m_swapChainCreateInfo.imageColorSpace) {
            found = true;
            break;
        }
    }
    if (!found)
        m_swapChainCreateInfo.setImageFormat(formats[0].format).setImageColorSpace(formats[0].colorSpace);

    auto resolution = m_window->osWindowResolution();
    resolution.width
        = std::clamp(resolution.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    resolution.height
        = std::clamp(resolution.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    m_swapChainCreateInfo.setMinImageCount(std::min(capabilities.minImageCount + 1, capabilities.maxImageCount))
        .setImageArrayLayers(1)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
        .setImageSharingMode(vk::SharingMode::eExclusive)
        .setClipped(true)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setPreTransform(capabilities.currentTransform)
        .setSurface(**m_window->m_surface)
        .setImageExtent(resolution);
    m_swapChain = std::make_unique<vk::raii::SwapchainKHR>(*getDevice()->getLogicalDevice(), m_swapChainCreateInfo);
    setFormat(m_swapChainCreateInfo.imageFormat);
    vk::Extent3D extent(m_swapChainCreateInfo.imageExtent);
    setDimensions(extent);
    setArrayLayers(m_swapChainCreateInfo.imageArrayLayers);
    setSamplesPerTexel(vk::SampleCountFlagBits::e1);
    auto images = m_swapChain->getImages();
    for (size_t i = 0; i < images.size(); i++) {
        m_images[i] = images[i];
        createView(i);
    }
    m_window->setSwapchainResolution(m_swapChainCreateInfo.imageExtent);

    m_recreate = false;
}

bool SwapChainImage::exists()
{
    return m_images.size();
}

uint32_t SwapChainImage::getNumberOfFramesInFlight()
{
    return m_images.size();
}

void SwapChainImage::recreate()
{
    m_recreate = true;
}
