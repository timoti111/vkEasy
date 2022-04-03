#pragma once

#include <vkEasy/global.h>

bool featuresAtLeastOneTrue(const vk::PhysicalDeviceFeatures& features);
vk::PhysicalDeviceFeatures featuresOr(const vk::PhysicalDeviceFeatures& lhs, const vk::PhysicalDeviceFeatures& rhs);
vk::PhysicalDeviceFeatures featuresAnd(const vk::PhysicalDeviceFeatures& lhs, const vk::PhysicalDeviceFeatures& rhs);
bool printUnsupported(const vk::PhysicalDeviceFeatures& device, const vk::PhysicalDeviceFeatures& needed);

namespace VK_EASY_NAMESPACE {
class Device;
class Graph;
class Object {
    friend class Graph;

public:
    Object() = default;
    Object(Graph* graph, Device* device)
        : m_graph(graph)
        , m_device(device) {};
    Graph* getGraph()
    {
        return m_graph;
    }
    Device* getDevice()
    {
        return m_device;
    }

private:
    Graph* m_graph = nullptr;
    Device* m_device = nullptr;
};
}