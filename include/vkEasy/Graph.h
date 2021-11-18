#pragma once

#include <vkEasy/Error.h>
#include <vkEasy/global.h>
#include <vkEasy/nodes/base/Node.h>
#include <vkEasy/resources/base/Resource.h>

namespace VK_EASY_NAMESPACE {
class Device;

class Graph : public Errorable {
    friend class Device;
    friend class Node;

public:
    Graph(Graph const&) = delete;
    void operator=(Graph const&) = delete;

    void startRecording();
    void stopRecording();
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
    void setDevice(Device* device);
    bool m_recording = false;

    std::vector<std::unique_ptr<Node>> m_nodes;
    std::vector<std::unique_ptr<Resource>> m_resources;

    std::vector<Node*> m_nodeOrderGraph;
    std::vector<std::function<void()>> m_callGraph;
    std::map<Resource*, std::vector<Node*>> m_resourceUsage;

    Device* m_device;
};
} // namespace VK_EASY_NAMESPACE
