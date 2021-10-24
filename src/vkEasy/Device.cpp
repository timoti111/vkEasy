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
    m_graphs.back()->setParent(this);
    return m_graphs.back().get();
}

vk::raii::Device* Device::getVkDevice()
{
    return m_device.get();
}

void Device::findPhysicalDevice()
{
    m_needsGraphicsQueue = false;
    m_graphicsQueueIndex = -1;
    m_needsComputeQueue = false;
    m_computeQueueIndex = -1;
    m_needsTransferQueue = false;
    m_transferQueueIndex = -1;
    m_needsPresentQueue = false;
    m_presentQueueIndex = -1;
    m_surface.reset();
    m_requiredExtensionsVkCompatible.clear();
    m_requiredExtensions.clear();
    m_requiredFeatures = vk::PhysicalDeviceFeatures();
    m_physicalDevice = &vk::easy::Context::get().getPhysicalDevices()[0];
}

void Device::initialize()
{
    m_queues.clear();
    m_device.reset();

    findPhysicalDevice();

    const float defaultQueuePriority(0.0f);
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    auto queueFamilyProperties = m_physicalDevice->getQueueFamilyProperties();

    int queueIndex = 0;
    for (auto& queueFamilyProperty : queueFamilyProperties) {
        bool useQueue = false;
        if (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics && m_needsGraphicsQueue) {
            m_graphicsQueueIndex = queueCreateInfos.size();
            useQueue = true;
        }
        if (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eCompute && m_needsComputeQueue) {
            m_computeQueueIndex = queueCreateInfos.size();
            useQueue = true;
        }
        if (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eTransfer && m_needsTransferQueue) {
            m_transferQueueIndex = queueCreateInfos.size();
            useQueue = true;
        }
        if (m_physicalDevice->getSurfaceSupportKHR(queueIndex, *m_surface) && m_needsPresentQueue) {
            m_presentQueueIndex = queueCreateInfos.size();
            useQueue = true;
        }

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

    for (auto& queue : queueCreateInfos)
        m_queues.push_back(std::move(std::make_unique<vk::raii::Queue>(*m_device, queue.queueFamilyIndex, 0)));

    if (m_needsGraphicsQueue && m_graphicsQueueIndex == -1 || m_needsComputeQueue && m_computeQueueIndex == -1
        || m_needsTransferQueue && m_transferQueueIndex == -1 || m_needsPresentQueue && m_presentQueueIndex == -1)
        error(Error::RequirementsNotFulfilled);
}