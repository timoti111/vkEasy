#include <iostream>
#include <limits>
#include <vkEasy/Context.h>
#include <vkEasy/Device.h>

using namespace VK_EASY_NAMESPACE;

Device::Device(vk::raii::PhysicalDevice* device)
    : Errorable("Device")
{
    m_physicalDevice = device;
}

Graph& Device::createGraph()
{
    m_graphs.push_back(std::unique_ptr<Graph>(new Graph()));
    m_graphs.back()->setDevice(this);
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
    m_universalQueueIndex = std::numeric_limits<size_t>::max();

    m_requiredExtensionsVkCompatible.clear();
    m_requiredExtensions.clear();
    m_requiredFeatures = vk::PhysicalDeviceFeatures();

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
        for (auto& node : graph->m_timeline) {
            m_neededQueues = m_neededQueues | node.renderTask->m_neededQueueTypes;
            for (auto& requiredExtension : node.renderTask->m_neededExtensions)
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
    for (auto& window : m_windows)
        window->update();

    m_queues.clear();
    m_device.reset();

    findPhysicalDevice();

    const float defaultQueuePriority(0.0f);
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    auto queueFamilyProperties = m_physicalDevice->getQueueFamilyProperties();
    m_queues.resize(queueFamilyProperties.size());

    int queueIndex = 0;
    for (auto& queueFamilyProperty : queueFamilyProperties) {
        bool useQueue = false;
        if ((queueFamilyProperty.queueFlags & m_neededQueues) == m_neededQueues) {
            if (!m_windows.empty() && !m_physicalDevice->getSurfaceSupportKHR(queueIndex, **m_windows[0]->m_surface))
                continue;
            m_universalQueueIndex = queueIndex;
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

    vk::CommandPoolCreateInfo cmdPoolInfo;
    for (auto& queue : queueCreateInfos) {
        auto queueData = std::make_unique<QueueData>();
        queueData->queue = std::make_unique<vk::raii::Queue>(*m_device, queue.queueFamilyIndex, 0);
        cmdPoolInfo.setQueueFamilyIndex(queue.queueFamilyIndex);
        queueData->commandPool = std::make_unique<vk::raii::CommandPool>(*m_device, cmdPoolInfo);
        m_queues.at(queue.queueFamilyIndex) = std::move(queueData);
    }

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
    allocatorInfo.vulkanApiVersion = Context::get().m_applicationInfo.apiVersion;
    allocatorInfo.physicalDevice = **m_physicalDevice;
    allocatorInfo.device = **m_device;
    allocatorInfo.instance = **Context::get().m_instance;
    m_allocator = std::make_unique<MemoryAllocator>(allocatorInfo, m_device.get(), Context::get().m_instance.get());
}

std::vector<vk::raii::CommandBuffer*> Device::getUniversalCommandBuffers(size_t count)
{
    return m_queues.at(m_universalQueueIndex)->getCommandBuffers(count, m_device.get());
}

void Device::sendCommandBuffers()
{
    for (auto& queue : m_queues)
        if (queue)
            queue->sendCommandBuffers(m_device.get());
}

void Device::resetCommandBuffers()
{
    for (auto& queue : m_queues)
        if (queue)
            queue->resetCommandBuffers();
}

void Device::waitForFences()
{
    for (auto& queue : m_queues)
        if (queue)
            queue->waitForFence(m_device.get());
}

void Device::wait()
{
    for (auto& queue : m_queues)
        if (queue)
            queue->waitIdle();
}

std::vector<vk::raii::CommandBuffer*> Device::QueueData::getCommandBuffers(size_t count, vk::raii::Device* device)
{
    if (count + usedCommandBuffers > allocatedCommandBuffers.size()) {
        vk::CommandBufferAllocateInfo cmdBufAllocateInfo;
        cmdBufAllocateInfo.setCommandPool(**commandPool)
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(count);
        commandBuffers.push_back(std::make_unique<vk::raii::CommandBuffers>(*device, cmdBufAllocateInfo));
        for (size_t i = 0; i < commandBuffers.back()->size(); i++)
            allocatedCommandBuffers.push_back(&commandBuffers.back()->at(i));
    }
    std::vector<vk::raii::CommandBuffer*> ret;
    ret.insert(ret.end(), allocatedCommandBuffers.begin() + usedCommandBuffers, allocatedCommandBuffers.end());
    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    for (auto& commandBuffer : ret)
        commandBuffer->begin(beginInfo);
    usedCommandBuffers += count;

    return ret;
}

void Device::QueueData::sendCommandBuffers(vk::raii::Device* device)
{
    std::vector<vk::CommandBuffer> cmdBuffersToSubmit;
    for (size_t i = 0; i < usedCommandBuffers; i++) {
        allocatedCommandBuffers[i]->end();
        cmdBuffersToSubmit.push_back(**allocatedCommandBuffers[i]);
    }
    usedCommandBuffers = 0;
    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(cmdBuffersToSubmit);

    if (!fence) {
        vk::FenceCreateInfo fenceInfo;
        fence = std::make_unique<vk::raii::Fence>(*device, fenceInfo);
    }
    device->resetFences(**fence);
    queue->submit(submitInfo, **fence);
}

void Device::QueueData::waitForFence(vk::raii::Device* device)
{
    device->waitForFences(**fence, VK_TRUE, UINT64_MAX);
}

void Device::QueueData::resetCommandBuffers()
{
    commandPool->reset();
}

void Device::QueueData::waitIdle()
{
    queue->waitIdle();
}

GLFWWindow& Device::createGLFWWindow(uint32_t width, uint32_t height, const std::string& title)
{
    return createWindow<GLFWWindow>(width, height, title);
}
