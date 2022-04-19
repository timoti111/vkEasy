#include <iostream>
#include <limits>
#include <vkEasy/Context.h>
#include <vkEasy/Device.h>
#include <vkEasy/WSI.h>

using namespace VK_EASY_NAMESPACE;

Device::Device(vk::raii::PhysicalDevice* device)
    : Errorable("Device")
{
    m_physicalDevice = device;
}

Graph& Device::createGraph()
{
    m_graphs.push_back(std::unique_ptr<Graph>(new Graph(this)));
    return *m_graphs.back().get();
}

vk::raii::Device* Device::getLogicalDevice()
{
    return m_device.get();
}

vk::raii::PhysicalDevice* Device::getPhysicalDevice()
{
    return m_physicalDevice;
}

MemoryAllocator* Device::getAllocator()
{
    return m_allocator.get();
}

void Device::findPhysicalDevice()
{
    m_neededQueues = vk::QueueFlags();
    m_queueIndex = std::numeric_limits<size_t>::max();

    m_requiredExtensionsVkCompatible.clear();
    m_requiredExtensions.clear();
    m_requiredFeatures = vk::PhysicalDeviceFeatures();

    for (auto& graph : m_graphs) {
        if (graph->m_window) {
            m_windows.push_back(graph->m_window.get());
            m_windows.back()->update();
        }
    }

    if (!m_physicalDevice) {
        std::multimap<int, vk::raii::PhysicalDevice*> candidates;
        for (auto& physicalDevice : vk::easy::Context::get().getPhysicalDevices()) {
            int score = 0;
            // score GPUs
            if (!m_windows.empty()) {
                auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
                for (size_t queueIndex = 0; queueIndex < queueFamilyProperties.size(); queueIndex++)
                    score += physicalDevice.getSurfaceSupportKHR(queueIndex, **m_windows[0]->m_surface) ? 1000 : 0;
            }
            candidates.emplace(score, &physicalDevice);
        }
        m_physicalDevice = candidates.rbegin()->second;
    }

    // auto deviceProperties = m_physicalDevice->getProperties();
    auto deviceExtensions = m_physicalDevice->enumerateDeviceExtensionProperties();

    auto addIfExists = [this, &deviceExtensions](const char* extName) {
        if (std::find_if(deviceExtensions.begin(), deviceExtensions.end(),
                [&extName](vk::ExtensionProperties const& ep) { return (strcmp(extName, ep.extensionName) == 0); })
            != deviceExtensions.end()) {
            m_requiredExtensions.emplace(extName);
        }
    };
    for (auto& graph : m_graphs)
        for (auto& node : graph->m_nodes) {
            m_neededQueues = m_neededQueues | node->m_neededQueueType;
            for (auto& requiredExtension : node->m_neededExtensions)
                m_requiredExtensions.insert(requiredExtension);
        }
    addIfExists(VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME);
    addIfExists(VK_KHR_BIND_MEMORY_2_EXTENSION_NAME);
    addIfExists(VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME);

    if (!m_windows.empty())
        addIfExists(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    std::transform(m_requiredExtensions.begin(), m_requiredExtensions.end(),
        std::back_inserter(m_requiredExtensionsVkCompatible),
        [](const std::string& string) -> char const* { return string.c_str(); });
}

void Device::initialize()
{
    Context::initialize();
    if (m_initialized)
        return; // TODO Error

    m_device.reset();

    findPhysicalDevice();

    const float defaultQueuePriority(0.0f);
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    auto queueFamilyProperties = m_physicalDevice->getQueueFamilyProperties();

    int queueIndex = 0;
    for (auto& queueFamilyProperty : queueFamilyProperties) {
        bool useQueue = false;
        if ((queueFamilyProperty.queueFlags & m_neededQueues) == m_neededQueues) {
            if (!m_windows.empty() && !m_physicalDevice->getSurfaceSupportKHR(queueIndex, **m_windows[0]->m_surface))
                continue;
            m_queueIndex = queueIndex;
            m_neededQueues &= ~queueFamilyProperty.queueFlags;
            useQueue = true;
        }

        if (useQueue) {
            vk::DeviceQueueCreateInfo queueCreateInfo;
            queueCreateInfo.setQueueFamilyIndex(queueIndex).setQueueCount(1).setPQueuePriorities(&defaultQueuePriority);
            queueCreateInfos.push_back(std::move(queueCreateInfo));
            break;
        }

        queueIndex++;
    }

    vk::DeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.setQueueCreateInfos(queueCreateInfos)
        .setPEnabledFeatures(&m_requiredFeatures)
        .setPEnabledExtensionNames(m_requiredExtensionsVkCompatible);
    m_device = std::make_unique<vk::raii::Device>(*m_physicalDevice, deviceCreateInfo);
    m_queue = std::make_unique<vk::raii::Queue>(*getLogicalDevice(), m_queueIndex, 0);

    if (m_neededQueues)
        error(Error::RequirementsNotFulfilled);

    initializeVMA();

#ifndef NDEBUG
    auto properties = m_physicalDevice->getProperties();
    auto version = properties.apiVersion;
    std::cout << "GPU instance initialised with: " << std::endl;
    std::cout << "Name: " << properties.deviceName.data() << std::endl;
    std::cout << "Version: " << VK_VERSION_MAJOR(version) << "." << VK_VERSION_MINOR(version) << "."
              << VK_VERSION_PATCH(version) << std::endl;
    std::for_each(m_requiredExtensions.begin(), m_requiredExtensions.end(),
        [](const auto& ext) { std::cout << "Device Extension: " << ext << std::endl; });
    std::cout << std::endl;
#endif

    m_initialized = true;
}

void Device::initializeVMA()
{
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.vulkanApiVersion = m_physicalDevice->getProperties().apiVersion;
    allocatorInfo.physicalDevice = **m_physicalDevice;
    allocatorInfo.device = **getLogicalDevice();
    allocatorInfo.instance = **Context::get().m_instance;
    m_allocator = std::make_unique<MemoryAllocator>(allocatorInfo, getLogicalDevice(), Context::get().m_instance.get());
}

vk::Result Device::present(vk::PresentInfoKHR* presentInfo)
{
    std::lock_guard<std::mutex> queueLock(m_queueMutex);
    return m_queue->presentKHR(*presentInfo);
}

void Device::wait()
{
    m_queue->waitIdle();
}

std::unique_ptr<vk::raii::CommandPool> Device::getCommandPool()
{
    vk::CommandPoolCreateInfo cmdPoolInfo;
    cmdPoolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    cmdPoolInfo.setQueueFamilyIndex(m_queueIndex);
    return std::make_unique<vk::raii::CommandPool>(*getLogicalDevice(), cmdPoolInfo);
}

void Device::sendCommandBuffers(vk::SubmitInfo* submitInfo, vk::raii::Fence* fence)
{
    std::lock_guard<std::mutex> queueLock(m_queueMutex);
    if (fence)
        m_queue->submit(*submitInfo, **fence);
    else
        m_queue->submit(*submitInfo);
}

uint32_t Device::getQueueIndex(vk::QueueFlagBits queueType)
{
    return m_queueIndex;
}
