#pragma once

#include <vkEasy/Error.h>
#include <vkEasy/global.h>
#include <vkEasy/nodes/Nodes.h>
#include <vkEasy/resources/Resources.h>

namespace VK_EASY_NAMESPACE {
class Device;

class Graph : public Errorable {
    friend class Device;
    friend class Resource;
    friend class Node;

public:
    Graph(Graph const&) = delete;
    void operator=(Graph const&) = delete;

    void enqueueNode(Node& node);
    void compile();
    void execute(bool waitUntilFinished = true);

    template <class T>
    requires(std::is_base_of_v<Node, T> && !std::is_same_v<Node, T>) T& createNode()
    {
        m_nodes.push_back(std::unique_ptr<T>(new T()));
        m_nodes.back()->setGraph(this);
        return *dynamic_cast<T*>(m_nodes.back().get());
    }
    GraphicsNode& createGraphicsNode();
    ComputeNode& createComputeNode();
    BufferCopyNode& createBufferCopyNode();
    MemoryReadNode& createMemoryReadNode();
    MemoryWriteNode& createMemoryWriteNode();
    PresentNode& createPresentNode();

    template <class T>
    requires(std::is_base_of_v<Resource, T> && !std::is_same_v<Resource, T>) T& createResource(
        Resource::OptimizationFlags optimization = Resource::OptimizationFlags::NO_OPTIMIZATION)
    {
        m_resources.push_back(std::unique_ptr<T>(new T()));
        m_resources.back()->setGraph(this);
        m_resources.back()->setOptimization(optimization);
        return *dynamic_cast<T*>(m_resources.back().get());
    }
    StagingBuffer& createStagingBuffer(
        Resource::OptimizationFlags optimization = Resource::OptimizationFlags::NO_OPTIMIZATION);
    StorageBuffer& createStorageBuffer(
        Resource::OptimizationFlags optimization = Resource::OptimizationFlags::NO_OPTIMIZATION);
    UniformBuffer& createUniformBuffer(
        Resource::OptimizationFlags optimization = Resource::OptimizationFlags::NO_OPTIMIZATION);

private:
    Graph();
    vk::raii::Event* createEvent(std::function<void()> action);
    void setDevice(Device* device);
    bool m_compiled = false;

    struct RenderStep {
        Node* renderTask;
        std::vector<Resource*> createdResources;
        std::vector<Resource*> destroyedResources;
    };
    std::vector<std::unique_ptr<Node>> m_nodes;
    std::vector<std::unique_ptr<Resource>> m_resources;
    std::vector<RenderStep> m_timeline;

    std::vector<Node*> m_nodeOrderGraph;

    struct Event {
        std::unique_ptr<vk::raii::Event> vkEvent;
        std::function<void()> action;
    };
    std::vector<Event> m_events;

    Device* m_device;
};
} // namespace VK_EASY_NAMESPACE
