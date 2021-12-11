#pragma once

#include <set>
#include <span>
#include <vkEasy/Error.h>
#include <vkEasy/Graph.h>
#include <vkEasy/MemoryAllocator.h>
#include <vkEasy/global.h>

namespace VK_EASY_NAMESPACE {
class Device : public Errorable {
    friend class Context;
    friend class Graph;

public:
    Device(Device const&) = delete;
    void operator=(Device const&) = delete;

    Graph& createGraph();
    vk::raii::Device* getLogicalDevice();
    vk::raii::PhysicalDevice* getPhysicalDevice();
    MemoryAllocator* getAllocator();

    void sendCommandBuffers();
    void waitForFences();
    void resetCommandBuffers();
    void waitForQueue();
    std::vector<vk::raii::CommandBuffer*> getUniversalCommandBuffers(size_t count);

private:
    Device() = delete;
    Device(vk::raii::PhysicalDevice* device);
    void findPhysicalDevice();
    void initialize();
    void initializeVMA();

    std::vector<char const*> m_requiredExtensionsVkCompatible;
    std::set<std::string> m_requiredExtensions;
    vk::PhysicalDeviceFeatures m_requiredFeatures;

    std::unique_ptr<vk::raii::Device> m_device;

    struct QueueData {
        std::unique_ptr<vk::raii::Queue> queue;
        std::unique_ptr<vk::raii::CommandPool> commandPool;
        std::vector<std::unique_ptr<vk::raii::CommandBuffers>> commandBuffers;
        std::vector<vk::raii::CommandBuffer*> allocatedCommandBuffers;
        size_t usedCommandBuffers = 0;
        std::unique_ptr<vk::raii::Fence> fence;

        std::vector<vk::raii::CommandBuffer*> getCommandBuffers(size_t count, vk::raii::Device* device);
        void sendCommandBuffers(vk::raii::Device* device);
        void waitForFence(vk::raii::Device* device);
        void resetCommandBuffers();
        void waitIdle();
    };
    std::vector<std::unique_ptr<QueueData>> m_queues;
    size_t m_universalQueueIndex;
    vk::QueueFlags m_neededQueues;

    std::unique_ptr<MemoryAllocator> m_allocator;
    std::vector<std::unique_ptr<Graph>> m_graphs;
    Graph* m_actualGraph;
    vk::raii::PhysicalDevice* m_physicalDevice = nullptr;
    bool m_initialized = false;
};
} // namespace VK_EASY_NAMESPACE
