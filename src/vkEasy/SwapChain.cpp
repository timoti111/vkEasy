#include <vkEasy/Device.h>
#include <vkEasy/SwapChain.h>
#include <vkEasy/WSI.h>

using namespace VK_EASY_NAMESPACE;

SwapChain::SwapChain(WSI* parent)
    : Errorable("SwapChain")
    , m_parent(parent)
{
    m_swapChainCreateInfo.setImageFormat(vk::Format::eB8G8R8A8Srgb);
}

void SwapChain::update()
{
    if (m_swapChain)
        return;

    m_buffers.clear();

    vk::raii::PhysicalDevice* device = m_parent->m_parent->m_physicalDevice;
    auto capabilities = device->getSurfaceCapabilitiesKHR(*m_parent->m_surface);
    auto formats = device->getSurfaceFormatsKHR(*m_parent->m_surface);
    auto presentModes = device->getSurfacePresentModesKHR(*m_parent->m_surface);
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

    auto resolution = m_parent->resolution();
    resolution.width
        = std::clamp(resolution.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    resolution.height
        = std::clamp(resolution.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    // m_swapChainCreateInfo.setMinImageCount(std::min(capabilities.minImageCount + 1, capabilities.maxImageCount));
    m_swapChainCreateInfo.setMinImageCount(1)
        .setImageArrayLayers(1)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
        .setImageSharingMode(vk::SharingMode::eExclusive)
        .setClipped(true)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setPreTransform(capabilities.currentTransform);

    m_swapChain = std::make_unique<vk::raii::SwapchainKHR>(*m_parent->m_parent->m_device, m_swapChainCreateInfo);

    auto images = m_swapChain->getImages();
    for (auto& image : images)
        m_buffers.push_back({ image, *m_parent->m_parent->m_device, this });
}

SwapChain::_SwapChainBuffers::_SwapChainBuffers(VkImage image, vk::raii::Device& device, SwapChain* swapChain)
{
    this->image = std::make_unique<vk::raii::Image>(device, image);

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
    viewCreateInfo.setImage(image)
        .setFormat(swapChain->m_swapChainCreateInfo.imageFormat)
        .setViewType(vk::ImageViewType::e2D)
        .setComponents(components)
        .setSubresourceRange(subresourceRange);
    this->view = std::make_unique<vk::raii::ImageView>(device, viewCreateInfo);

    vk::SemaphoreCreateInfo semaphoreCreateInfo;
    this->imageAvailableSemaphore = std::make_unique<vk::raii::Semaphore>(device, semaphoreCreateInfo);
    this->renderFinishedSemaphore = std::make_unique<vk::raii::Semaphore>(device, semaphoreCreateInfo);

    vk::FenceCreateInfo fenceCreateInfo;
    fenceCreateInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
    this->inFlightFence = std::make_unique<vk::raii::Fence>(device, fenceCreateInfo);
}
