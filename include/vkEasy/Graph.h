#pragma once

#include <vkEasy/Node.h>
#include <vkEasy/Resource.h>
#include <vkEasy/global.h>

namespace VK_EASY_NAMESPACE {
class Graph {
public:
    void record();
    void run();

    template <class T>
    requires(std::is_base_of_v<Node, T> && !std::is_same_v<Node, T>) T* createNode()
    {
        m_nodes.push_back(std::make_unique<T>());
        return dynamic_cast<T*>(m_nodes[m_nodes.size() - 1].get());
    }

    template <class T>
    requires(std::is_base_of_v<Resource, T> && !std::is_same_v<Resource, T>) T* createResource()
    {
        m_resources.push_back(std::make_unique<T>());
        return dynamic_cast<T*>(m_resources.back().get());
    }

private:
    std::vector<std::unique_ptr<Node>> m_nodes;
    std::vector<std::unique_ptr<Resource>> m_resources;
};
} // namespace VK_EASY_NAMESPACE
