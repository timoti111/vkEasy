#pragma once

#include <vkEasy/Error.h>
#include <vkEasy/global.h>
#include <vkEasy/nodes/base/Node.h>
#include <vkEasy/resources/base/Resource.h>

namespace VK_EASY_NAMESPACE {
class Context;

class Graph : public Errorable {
    friend class Context;
    friend class Node;

public:
    Graph(Graph const&) = delete;
    void operator=(Graph const&) = delete;

    void setPhysicalDevice(vk::raii::PhysicalDevice* physicalDevice = nullptr);
    void startBuilding();
    void stopBuilding();
    void run();

    template <class T>
    requires(std::is_base_of_v<Node, T> && !std::is_same_v<Node, T>) T* createNode()
    {
        m_nodes.push_back(std::unique_ptr<T>(new T()));
        m_nodes.back()->setParent(this);
        return dynamic_cast<T*>(m_nodes.back().get());
    }

    template <class T>
    requires(std::is_base_of_v<Resource, T> && !std::is_same_v<Resource, T>) T* createResource()
    {
        m_resources.push_back(std::unique_ptr<T>(new T()));
        return dynamic_cast<T*>(m_resources.back().get());
    }

private:
    Graph();
    void setParent(Context* context);
    void addToGraph(Node* node);
    bool m_building = false;

    std::vector<std::unique_ptr<Node>> m_nodes;
    std::vector<std::unique_ptr<Resource>> m_resources;

    std::vector<Node*> m_graph;

    vk::raii::PhysicalDevice* m_physicalDevice = nullptr;
    vk::raii::Device* m_device = nullptr;
    Context* m_parent;
};
} // namespace VK_EASY_NAMESPACE
