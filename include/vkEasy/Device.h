#pragma once

#include <set>
#include <vkEasy/Error.h>
#include <vkEasy/Graph.h>
#include <vkEasy/global.h>

namespace VK_EASY_NAMESPACE {
class Device : public Errorable {
    friend class Context;
    friend class Graph;

public:
    Device(Device const&) = delete;
    void operator=(Device const&) = delete;
    Graph* createGraph();
    vk::raii::Device* getVkDevice();

private:
    Device() = delete;
    Device(vk::raii::PhysicalDevice* device);
    void findDevice();
    void initialize();

    std::set<std::string> m_requiredExtensions;
    vk::PhysicalDeviceFeatures m_requiredFeatures;

    std::unique_ptr<vk::raii::Device> m_device;
    std::vector<std::unique_ptr<Graph>> m_graphs;
    vk::raii::PhysicalDevice* m_explicitPhysicalDevice = nullptr;
    bool m_initialized = false;
};
} // namespace VK_EASY_NAMESPACE
