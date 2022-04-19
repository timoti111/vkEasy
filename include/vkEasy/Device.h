#pragma once

#include <vkEasy/Error.h>
#include <vkEasy/Graph.h>
#include <vkEasy/MemoryAllocator.h>
#include <vkEasy/global.h>

namespace VK_EASY_NAMESPACE {
class Device : public Errorable {
    friend class Context;
    friend class Graph;
    friend class Node;

public:
    Device(Device const&) = delete;
    void operator=(Device const&) = delete;

    Graph& createGraph();
    vk::raii::Device* getLogicalDevice();
    vk::raii::PhysicalDevice* getPhysicalDevice();
    MemoryAllocator* getAllocator();

    void wait();
    void initialize();

private:
    Device() = delete;
    Device(vk::raii::PhysicalDevice* device);
    void findPhysicalDevice();
    void initializeVMA();
    uint32_t getQueueIndex(vk::QueueFlagBits queueType);

    std::vector<char const*> m_requiredExtensionsVkCompatible;
    std::set<std::string> m_requiredExtensions;
    vk::PhysicalDeviceFeatures m_requiredFeatures;

    std::unique_ptr<vk::raii::Device> m_device;
    std::unique_ptr<vk::raii::Queue> m_queue;
    std::mutex m_queueMutex;
    std::unique_ptr<vk::raii::CommandPool> getCommandPool();
    vk::Result present(vk::PresentInfoKHR* presentInfo);
    void sendCommandBuffers(vk::SubmitInfo* submitInfo, vk::raii::Fence* fence = nullptr);
    size_t m_queueIndex;
    vk::QueueFlags m_neededQueues;

    std::unique_ptr<MemoryAllocator> m_allocator;
    std::vector<std::unique_ptr<Graph>> m_graphs;
    std::vector<WSI*> m_windows;
    vk::raii::PhysicalDevice* m_physicalDevice = nullptr;
    bool m_initialized = false;
};
} // namespace VK_EASY_NAMESPACE
