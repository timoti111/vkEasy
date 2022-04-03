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

VkImage SwapChainImage::getVkImage()
{
    return m_images[0];
}

vk::raii::ImageView* SwapChainImage::getVkImageView(uint32_t imageIndex)
{
    return m_views[imageIndex].get();
}

void SwapChainImage::destroy()
{
}

void SwapChainImage::update()
{
    if (m_swapChain)
        return;

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

    auto resolution = m_window->resolution();
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
        .setImageExtent(resolution)
        .setPresentMode(vk::PresentModeKHR::eFifo);

    m_swapChain = std::make_unique<vk::raii::SwapchainKHR>(*getDevice()->getLogicalDevice(), m_swapChainCreateInfo);

    m_images = m_swapChain->getImages();
    setFormat(m_swapChainCreateInfo.imageFormat);
    vk::Extent3D extent(m_swapChainCreateInfo.imageExtent);
    setDimensions(extent);
    setArrayLayers(m_swapChainCreateInfo.imageArrayLayers);
    setSamplesPerTexel(vk::SampleCountFlagBits::e1);

    vk::ImageViewCreateInfo viewCreateInfo;
    vk::ComponentMapping components;
    components.setR(vk::ComponentSwizzle::eIdentity)
        .setG(vk::ComponentSwizzle::eIdentity)
        .setB(vk::ComponentSwizzle::eIdentity)
        .setA(vk::ComponentSwizzle::eIdentity);
    vk::ImageSubresourceRange subresourceRange;
    subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor)
        .setBaseMipLevel(0)
        .setLevelCount(1)
        .setBaseArrayLayer(0)
        .setLayerCount(1);
    viewCreateInfo.setFormat(getFormat())
        .setViewType(vk::ImageViewType::e2D)
        .setComponents(components)
        .setSubresourceRange(subresourceRange);

    for (auto& image : m_images) {
        viewCreateInfo.setImage(image);
        m_views.push_back(std::make_unique<vk::raii::ImageView>(*getDevice()->getLogicalDevice(), viewCreateInfo));
    }
}

bool SwapChainImage::exists()
{
    return m_images.size();
}

uint32_t SwapChainImage::getNumberOfFramesInFlight()
{
    return m_images.size();
}
