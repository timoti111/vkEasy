#pragma once

#include "vkEasy/SwapChain.h"
#include <vkEasy/Error.h>
#include <vkEasy/GLFWWindow.h>
#include <vkEasy/Graph.h>
#include <vkEasy/MemoryAllocator.h>
#include <vkEasy/global.h>

namespace VK_EASY_NAMESPACE {
class Device : public Errorable {
    friend class Context;
    friend class Graph;
    friend class SwapChain;

public:
    Device(Device const&) = delete;
    void operator=(Device const&) = delete;

    template <class T>
    requires(std::is_base_of_v<WSI, T> && !std::is_same_v<WSI, T>) T& createWindow(
        uint32_t width, uint32_t height, const std::string& title)
    {
        m_windows.push_back(std::unique_ptr<T>(new T(width, height, title, this)));
        return *dynamic_cast<T*>(m_windows.back().get());
    }
    GLFWWindow& createGLFWWindow(uint32_t width, uint32_t height, const std::string& title);

    Graph& createGraph();
    vk::raii::Device* getLogicalDevice();
    vk::raii::PhysicalDevice* getPhysicalDevice();
    MemoryAllocator* getAllocator();

    void sendCommandBuffers();
    void waitForFences();
    void resetCommandBuffers();
    void wait();
    std::vector<vk::raii::CommandBuffer*> getUniversalCommandBuffers(size_t count);

private:
    Device() = delete;
    Device(vk::raii::PhysicalDevice* device);
    void findPhysicalDevice();
    void initialize();
    void initializeVMA();

    std::vector<std::unique_ptr<WSI>> m_windows;

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
    vk::raii::PhysicalDevice* m_physicalDevice = nullptr;
    bool m_initialized = false;
};
} // namespace VK_EASY_NAMESPACE
