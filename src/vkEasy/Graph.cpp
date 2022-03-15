#include <algorithm>
#include <iostream>
#include <thread>
#include <vkEasy/Context.h>
#include <vkEasy/Graph.h>

using namespace VK_EASY_NAMESPACE;

Graph::Graph()
    : Errorable("Graph")
{
}

void Graph::startRecording()
{
    m_nodeOrderGraph.clear();
    m_resourceUsage.clear();
    m_callGraph.clear();
    m_recording = true;
}

void Graph::stopRecording()
{
    m_recording = false;
    auto resourceUsageCopy = m_resourceUsage;
    for (auto& node : m_nodeOrderGraph)
        m_callGraph.push_back([=]() { node->execute(); });
}

void Graph::run()
{
    if (m_device->m_actualGraph != this) {
        m_device->m_actualGraph = this;
        m_device->initialize();
    }

#ifndef NDEBUG
    std::cout << "Executing graph:" << std::endl;
#endif

    if (m_recording)
        error(Error::RecordingGraph);

    m_events.clear();
    setActualPipelineStage(vk::PipelineStageFlagBits::eNoneKHR);

    for (auto& action : m_callGraph)
        action();
    m_device->sendCommandBuffers();

    for (auto& event : m_events) {
        while (event.vkEvent->getStatus() != vk::Result::eEventSet)
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        event.action();
    }

#ifndef NDEBUG
    std::cout << std::endl;
#endif
}

void Graph::setDevice(Device* device)
{
    if (m_device == device)
        return;
    m_device = device;
    for (auto& node : m_nodes)
        node->setGraph(this);
}

vk::raii::Event* Graph::createEvent(std::function<void()> action)
{
    vk::EventCreateInfo info;
    m_events.push_back(Event());
    m_events.back().vkEvent = std::make_unique<vk::raii::Event>(*m_device->getLogicalDevice(), info);
    m_events.back().action = action;
    return m_events.back().vkEvent.get();
}

vk::PipelineStageFlagBits Graph::getLastPipelineStage()
{
    return m_lastPipelineStage;
}

void Graph::setActualPipelineStage(vk::PipelineStageFlagBits stage)
{
    m_lastPipelineStage = stage;
}

GraphicsNode& Graph::createGraphicsNode()
{
    return this->createNode<GraphicsNode>();
}

ComputeNode& Graph::createComputeNode()
{
    return this->createNode<ComputeNode>();
}

BufferCopyNode& Graph::createBufferCopyNode()
{
    return this->createNode<BufferCopyNode>();
}

MemoryReadNode& Graph::createMemoryReadNode()
{
    return this->createNode<MemoryReadNode>();
}

MemoryWriteNode& Graph::createMemoryWriteNode()
{
    return this->createNode<MemoryWriteNode>();
}

PresentNode& Graph::createPresentNode()
{
    return this->createNode<PresentNode>();
}

StagingBuffer& Graph::createStagingBuffer()
{
    return this->createResource<StagingBuffer>();
}

StorageBuffer& Graph::createStorageBuffer()
{
    return this->createResource<StorageBuffer>();
}

UniformBuffer& Graph::createUniformBuffer()
{
    return this->createResource<UniformBuffer>();
}
