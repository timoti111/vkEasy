#pragma once

#include <vkEasy/Error.h>
#include <vkEasy/Framebuffer.h>
#include <vkEasy/GLFWWindow.h>
#include <vkEasy/Utils.h>
#include <vkEasy/global.h>
#include <vkEasy/nodes/Nodes.h>
#include <vkEasy/resources/Resources.h>

namespace VK_EASY_NAMESPACE {
class Device;

class Graph : public Errorable, public Object {
    friend class Device;
    friend class Node;
    friend class Resource;
    friend class Framebuffer;

public:
    ~Graph();
    Graph(Graph const&) = delete;
    void operator=(Graph const&) = delete;

    void enqueueNode(Node& node);
    void compile();
    void execute();

    template <class T>
    requires(std::is_base_of_v<Node, T> && !std::is_same_v<Node, T>) T& createNode()
    {
        m_nodes.push_back(std::unique_ptr<T>(new T()));
        m_nodes.back()->m_graph = getGraph();
        m_nodes.back()->m_device = getDevice();
        return *dynamic_cast<T*>(m_nodes.back().get());
    }
    GraphicsNode& createGraphicsNode();
    ComputeNode& createComputeNode();
    BufferCopyNode& createBufferCopyNode();

    template <class T>
    requires(std::is_base_of_v<Resource, T> && !std::is_same_v<Resource, T>) T& createResource(
        Resource::OptimizationFlags optimization = Resource::OptimizationFlags::NO_OPTIMIZATION)
    {
        m_resources.push_back(std::unique_ptr<T>(new T()));
        m_resources.back()->setOptimization(optimization);
        m_resources.back()->m_graph = getGraph();
        m_resources.back()->m_device = getDevice();
        return *dynamic_cast<T*>(m_resources.back().get());
    }
    StagingBuffer& createStagingBuffer(
        Resource::OptimizationFlags optimization = Resource::OptimizationFlags::NO_OPTIMIZATION);
    StorageBuffer& createStorageBuffer(
        Resource::OptimizationFlags optimization = Resource::OptimizationFlags::NO_OPTIMIZATION);
    UniformBuffer& createUniformBuffer(
        Resource::OptimizationFlags optimization = Resource::OptimizationFlags::NO_OPTIMIZATION);
    VertexBuffer& createVertexBuffer(
        Resource::OptimizationFlags optimization = Resource::OptimizationFlags::NO_OPTIMIZATION);
    IndexBuffer& createIndexBuffer(
        Resource::OptimizationFlags optimization = Resource::OptimizationFlags::NO_OPTIMIZATION);

    Framebuffer& createFramebuffer();

    GLFWWindow& getGLFWWindow(uint32_t width, uint32_t height, const std::string& title);
    void setNumberOfFramesInFlight(size_t count);

private:
    Graph(Device* device);
    void pushCommand(std::function<void()> command);
    void createSynchronizationObjects();
    uint32_t getImageIndex();
    uint32_t getNumberOfImages();
    uint32_t getCurrentFrameInFlight();
    uint32_t getNumberOfFramesInFlight();
    bool m_compiled = false;
    struct CommandBuffers {
        std::unique_ptr<vk::raii::CommandPool> commandPool;
        std::vector<std::unique_ptr<vk::raii::CommandBuffers>> commandBuffers;
        std::vector<vk::raii::CommandBuffer*> allocatedCommandBuffers;
        size_t usedCommandBuffers = 0;

        std::vector<vk::raii::CommandBuffer*> getCommandBuffers(size_t count, vk::raii::Device* device);
        std::vector<vk::CommandBuffer> endCommandBuffers();
        void resetCommandBuffers();
    };
    std::vector<vk::raii::CommandBuffer*> getCommandBuffers(size_t count);

    struct RenderStep {
        Node* renderTask;
        std::vector<Resource*> createdResources;
        std::vector<Resource*> destroyedResources;
    };
    std::vector<RenderStep> m_timeline;

    std::map<size_t, CommandBuffers> m_commandBuffers;
    std::map<size_t, std::unique_ptr<vk::raii::Semaphore>> m_imageAvailableSemaphore;
    std::map<size_t, std::unique_ptr<vk::raii::Semaphore>> m_renderFinishedSemaphore;
    std::map<size_t, std::unique_ptr<vk::raii::Fence>> m_inFlightFence;
    std::unique_ptr<WSI> m_window;
    std::vector<std::unique_ptr<Resource>> m_resources;
    std::vector<std::unique_ptr<Framebuffer>> m_framebuffers;
    std::vector<std::unique_ptr<Node>> m_nodes;

    std::vector<Node*> m_nodeOrderGraph;
    std::vector<std::function<void()>> m_commands;

    size_t m_framesInFlight = 1;
    size_t m_currentFrameInFlight = 0;
    uint32_t m_imageIndex;
};
} // namespace VK_EASY_NAMESPACE
