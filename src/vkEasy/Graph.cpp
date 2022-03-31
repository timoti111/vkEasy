#include <algorithm>
#include <fstream>
#include <iostream>
#include <stack>
#include <thread>
#include <vkEasy/Context.h>
#include <vkEasy/Graph.h>

using namespace VK_EASY_NAMESPACE;

Graph::Graph()
    : Errorable("Graph")
{
}

void Graph::enqueueNode(Node& node)
{
    if (node.m_graph != this || m_compiled)
        return; // TODO Error

    m_nodeOrderGraph.push_back(&node);
}

void Graph::compile()
{
    if (m_compiled)
        return; // TODO Error

    for (auto& node : m_nodeOrderGraph) {
        for (auto& writeResource : node->m_writes) {
            if (!writeResource->m_creator && writeResource->m_graph == this) {
                writeResource->m_creator = node;
                node->m_creates.insert(writeResource);
            }
        }
        for (auto& createResource : node->m_creates) {
            node->m_reads.erase(createResource);
            node->m_writes.erase(createResource);
        }
        for (auto& readResource : node->m_reads)
            readResource->m_readers.push_back(node);
        for (auto& writeResource : node->m_writes)
            writeResource->m_writers.push_back(node);
    }

    // Reference counting.
    for (auto& node : m_nodeOrderGraph)
        node->m_referenceCount = node->m_creates.size() + node->m_writes.size();
    for (auto& resource : m_resources)
        resource->m_referenceCount = resource->m_readers.size();

    // Culling via flood fill from unreferenced resources.
    auto isTransient = [this](Resource* resource) -> bool {
        return resource->m_creator && resource->m_creator->m_graph == this && !resource->m_isPersistent;
    };

    std::stack<Resource*> unreferencedResources;
    for (auto& resource : m_resources)
        if (resource->m_referenceCount == 0 && isTransient(resource.get()))
            unreferencedResources.push(resource.get());
    while (!unreferencedResources.empty()) {
        auto unreferencedResource = unreferencedResources.top();
        unreferencedResources.pop();

        auto creator = unreferencedResource->m_creator;
        if (creator->m_referenceCount > 0)
            creator->m_referenceCount--;
        if (creator->m_referenceCount == 0 && !creator->m_cullImmune) {
            for (auto& readResource : creator->m_reads) {
                if (readResource->m_referenceCount > 0)
                    readResource->m_referenceCount--;
                if (readResource->m_referenceCount == 0 && isTransient(readResource))
                    unreferencedResources.push(readResource);
            }
        }

        for (auto& writer : unreferencedResource->m_writers) {
            if (writer->m_referenceCount > 0)
                writer->m_referenceCount--;
            if (writer->m_referenceCount == 0 && !writer->m_cullImmune) {
                for (auto& readResource : writer->m_reads) {
                    if (readResource->m_referenceCount > 0)
                        readResource->m_referenceCount--;
                    if (readResource->m_referenceCount == 0 && isTransient(readResource))
                        unreferencedResources.push(readResource);
                }
            }
        }
    }

    // Timeline computation.
    m_timeline.clear();
    for (auto& node : m_nodeOrderGraph) {
        if (node->m_referenceCount == 0 && !node->m_cullImmune)
            continue;

        std::vector<Resource*> createdResources, destroyedResources;

        for (auto& resource : node->m_creates) {
            createdResources.push_back(const_cast<Resource*>(resource));
            if (resource->m_readers.empty() && resource->m_writers.empty())
                destroyedResources.push_back(const_cast<Resource*>(resource));
        }

        auto readsWrites = node->m_reads;
        readsWrites.insert(node->m_writes.begin(), node->m_writes.end());
        for (auto& resource : readsWrites) {
            if (!isTransient(resource))
                continue;

            auto valid = false;
            std::size_t lastIndex;
            if (!resource->m_readers.empty()) {
                auto lastReader = std::find_if(m_nodeOrderGraph.begin(), m_nodeOrderGraph.end(),
                    [&resource](const auto& node) { return node == resource->m_readers.back(); });
                if (lastReader != m_nodeOrderGraph.end()) {
                    valid = true;
                    lastIndex = std::distance(m_nodeOrderGraph.begin(), lastReader);
                }
            }
            if (!resource->m_writers.empty()) {
                auto lastWriter = std::find_if(m_nodeOrderGraph.begin(), m_nodeOrderGraph.end(),
                    [&resource](const auto& node) { return node == resource->m_writers.back(); });
                if (lastWriter != m_nodeOrderGraph.end()) {
                    valid = true;
                    lastIndex = std::max(lastIndex, std::size_t(std::distance(m_nodeOrderGraph.begin(), lastWriter)));
                }
            }

            if (valid && m_nodeOrderGraph[lastIndex] == node)
                destroyedResources.push_back(const_cast<Resource*>(resource));
        }

        m_timeline.push_back(RenderStep { node, createdResources, destroyedResources });
    }
    m_compiled = true;
}

void Graph::execute(bool waitUntilFinished)
{
    if (!m_device->m_initialized)
        m_device->initialize();

    if (!m_compiled)
        error(Error::RecordingGraph);

#ifndef NDEBUG
    std::cout << "Executing graph:" << std::endl;
#endif

    m_events.clear();
    for (auto& step : m_timeline) {
        for (auto& resource : step.createdResources)
            resource->update();
        step.renderTask->execute();
        // for (auto resource : step.destroyedResources)   // TODO
        //     resource->destroy();
    }
    m_device->sendCommandBuffers();

    for (auto& event : m_events) {
        while (event.vkEvent->getStatus() != vk::Result::eEventSet)
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        event.action();
    }

#ifndef NDEBUG
    std::cout << std::endl;
#endif

    if (waitUntilFinished)
        m_device->wait();
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

StagingBuffer& Graph::createStagingBuffer(Resource::OptimizationFlags optimization)
{
    return this->createResource<StagingBuffer>(optimization);
}

StorageBuffer& Graph::createStorageBuffer(Resource::OptimizationFlags optimization)
{
    return this->createResource<StorageBuffer>(optimization);
}

UniformBuffer& Graph::createUniformBuffer(Resource::OptimizationFlags optimization)
{
    return this->createResource<UniformBuffer>(optimization);
}
