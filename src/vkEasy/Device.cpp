#include <vkEasy/Context.h>
#include <vkEasy/Device.h>

using namespace VK_EASY_NAMESPACE;

Device::Device(vk::raii::PhysicalDevice* device)
    : Errorable("Device")
{
}

Graph* Device::createGraph()
{
    m_graphs.push_back(std::unique_ptr<Graph>(new Graph()));
    m_graphs.back()->setDevice(this);
    return m_graphs.back().get();
}

vk::raii::Device* Device::getVkDevice()
{
    return m_device.get();
}

void Device::findPhysicalDevice()
{
    // TODO ALL
    m_needsGraphicsQueue = false;
    m_graphicsQueueIndex = -1;
    m_needsComputeQueue = true; // TODO
    m_computeQueueIndex = -1;
    m_needsTransferQueue = true; // TODO
    m_transferQueueIndex = -1;
    m_needsPresentQueue = false;
    m_presentQueueIndex = -1;
    // m_surface.reset();
    m_requiredExtensionsVkCompatible.clear();
    m_requiredExtensions.clear();
    m_requiredFeatures = vk::PhysicalDeviceFeatures();

    // if (!m_physicalDevice) {
    //     std::multimap<int, vk::PhysicalDevice*> candidates;
    //     for (auto& physicalDevice : vk::easy::Context::get().getPhysicalDevices()) { }
    // }
    if (!m_physicalDevice)
        m_physicalDevice = &vk::easy::Context::get().getPhysicalDevices()[0];

    // std::transform(m_requiredExtensions.begin(), m_requiredExtensions.end(),
    //     std::back_inserter(m_requiredExtensionsVkCompatible),
    //     [](const std::string& string) -> char const* { return string.c_str(); });
}

void Device::initialize()
{
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
        if (m_graphicsQueueIndex == -1 && queueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics
            && m_needsGraphicsQueue) {
            m_graphicsQueueIndex = queueIndex;
            useQueue = true;
        }
        if (m_computeQueueIndex == -1 && queueFamilyProperty.queueFlags & vk::QueueFlagBits::eCompute
            && m_needsComputeQueue) {
            m_computeQueueIndex = queueIndex;
            useQueue = true;
        }
        if (m_transferQueueIndex == -1 && queueFamilyProperty.queueFlags & vk::QueueFlagBits::eTransfer
            && m_needsTransferQueue) {
            m_transferQueueIndex = queueIndex;
            useQueue = true;
        }
        // if (m_presentQueueIndex == -1 && m_physicalDevice->getSurfaceSupportKHR(queueIndex, *m_surface) &&
        // m_needsPresentQueue) {
        //     m_presentQueueIndex = queueIndex;
        //     useQueue = true;
        // }

        if (useQueue) {
            vk::DeviceQueueCreateInfo queueCreateInfo;
            queueCreateInfo.setQueueFamilyIndex(queueIndex).setQueueCount(1).setPQueuePriorities(&defaultQueuePriority);
            queueCreateInfos.push_back(std::move(queueCreateInfo));
        }

        queueIndex++;
    }

    vk::DeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.setQueueCreateInfos(queueCreateInfos)
        .setPEnabledFeatures(&m_requiredFeatures)
        .setPEnabledExtensionNames(m_requiredExtensionsVkCompatible);
    m_device = std::make_unique<vk::raii::Device>(*m_physicalDevice, deviceCreateInfo);

    vk::CommandPoolCreateInfo cmdPoolInfo;
    cmdPoolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    for (auto& queue : queueCreateInfos) {
        auto queueData = std::make_unique<QueueData>();
        queueData->queue = std::make_unique<vk::raii::Queue>(*m_device, queue.queueFamilyIndex, 0);
        cmdPoolInfo.setQueueFamilyIndex(queue.queueFamilyIndex);
        queueData->commandPool = std::make_unique<vk::raii::CommandPool>(*m_device, cmdPoolInfo);
        m_queues.at(queue.queueFamilyIndex) = std::move(queueData);
    }

    if (m_needsGraphicsQueue && m_graphicsQueueIndex == -1 || m_needsComputeQueue && m_computeQueueIndex == -1
        || m_needsTransferQueue && m_transferQueueIndex == -1 || m_needsPresentQueue && m_presentQueueIndex == -1)
        error(Error::RequirementsNotFulfilled);
}

std::span<vk::raii::CommandBuffer*> Device::getCommandBuffers(size_t count, vk::QueueFlagBits queueType)
{
    switch (queueType) {
    case vk::QueueFlagBits::eCompute:
        return m_queues.at(m_computeQueueIndex)->getCommandBuffers(count, m_device.get());
    case vk::QueueFlagBits::eGraphics:
        return m_queues.at(m_graphicsQueueIndex)->getCommandBuffers(count, m_device.get());
    case vk::QueueFlagBits::eTransfer:
        return m_queues.at(m_transferQueueIndex)->getCommandBuffers(count, m_device.get());
    default:
        break;
    }
    return std::span<vk::raii::CommandBuffer*>();
}

void Device::sendCommandBuffers()
{
    for (auto& queue : m_queues)
        queue->sendCommandBuffers(m_device.get());
}

void Device::resetCommandBuffers()
{
    for (auto& queue : m_queues)
        queue->resetCommandBuffers();
}

void Device::waitForFences()
{
    for (auto& queue : m_queues)
        queue->waitForFence(m_device.get());
}

std::span<vk::raii::CommandBuffer*> Device::QueueData::getCommandBuffers(size_t count, vk::raii::Device* device)
{
    if (count + usedCommandBuffers > allocatedCommandBuffers.size()) {
        vk::CommandBufferAllocateInfo cmdBufAllocateInfo;
        cmdBufAllocateInfo.setCommandPool(**commandPool)
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(count);
        auto actualCommandBuffersSize = commandBuffers.size();
        commandBuffers.push_back(std::make_unique<vk::raii::CommandBuffers>(*device, cmdBufAllocateInfo));
        for (size_t i = actualCommandBuffersSize; i < commandBuffers.size(); i++)
            allocatedCommandBuffers.push_back(&commandBuffers.back()->at(i));
    }
    auto ret = std::span<vk::raii::CommandBuffer*>(allocatedCommandBuffers.begin() + usedCommandBuffers, count);
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
    for (size_t i = 0; i < usedCommandBuffers; i++)
        cmdBuffersToSubmit.push_back(**allocatedCommandBuffers[i]);
    vk::SubmitInfo submitInfo;
    submitInfo.setCommandBuffers(cmdBuffersToSubmit);
    // Submit to the queue
    vk::FenceCreateInfo fenceInfo;
    fence = std::make_unique<vk::raii::Fence>(*device, fenceInfo);
    queue->submit(submitInfo, **fence);
}

void Device::QueueData::waitForFence(vk::raii::Device* device)
{
    device->waitForFences(**fence, VK_TRUE, UINT64_MAX);
}

void Device::QueueData::resetCommandBuffers()
{
    for
}