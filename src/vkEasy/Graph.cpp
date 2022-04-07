#include <algorithm>
#include <fstream>
#include <iostream>
#include <stack>
#include <thread>
#include <vkEasy/Context.h>
#include <vkEasy/Graph.h>

using namespace VK_EASY_NAMESPACE;

Graph::Graph(Device* device)
    : Errorable("Graph")
    , Object(this, device)
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

        if (auto graphicsNode = dynamic_cast<GraphicsNode*>(node))
            graphicsNode->inOrder();

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

void Graph::execute()
{
    if (!getDevice()->m_initialized)
        getDevice()->initialize();
    if (!m_compiled)
        error(Error::RecordingGraph);

    if (m_window) {
        m_window->pollEvents();
        m_window->m_swapChain->update();
    }
    createSynchronizationObjects();

#ifndef NDEBUG
    std::cout << "Executing graph:" << std::endl;
#endif

    vk::SubmitInfo submitInfo;
    vk::PipelineStageFlags dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    getDevice()->getLogicalDevice()->waitForFences(**m_inFlightFence[m_currentFramesInFlight], true, UINT64_MAX);

    if (m_window) {
        auto result = m_window->m_swapChain->m_swapChain->acquireNextImage(
            UINT64_MAX, **m_imageAvailableSemaphore[m_currentFramesInFlight]);
        m_imageIndex = result.second;
        if (result.first == vk::Result::eErrorOutOfDateKHR) {
            m_window->recreateSwapchain();
            return;
        }
        submitInfo.setWaitSemaphores(**m_imageAvailableSemaphore[m_currentFramesInFlight])
            .setWaitDstStageMask(dstStageMask)
            .setSignalSemaphores(**m_renderFinishedSemaphore[m_currentFramesInFlight]);
    }

    getDevice()->getLogicalDevice()->resetFences(**m_inFlightFence[m_currentFramesInFlight]);
    m_commandBuffers[m_currentFramesInFlight].resetCommandBuffers();

    m_events.clear();
    for (auto& step : m_timeline) {
        for (auto& resource : step.createdResources)
            resource->update();
        step.renderTask->execute();
        // for (auto resource : step.destroyedResources)   // TODO
        //     resource->destroy();
    }

    auto buffers = m_commandBuffers[m_currentFramesInFlight].endCommandBuffers();
    submitInfo.setCommandBuffers(buffers);
    getDevice()->sendCommandBuffers(&submitInfo, m_inFlightFence[m_currentFramesInFlight].get());

    for (auto& event : m_events) {
        while (event.vkEvent->getStatus() != vk::Result::eEventSet)
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        event.action();
    }

    if (m_window) {
        vk::PresentInfoKHR presentInfo;
        presentInfo.setImageIndices(m_imageIndex)
            .setSwapchains(**m_window->m_swapChain->m_swapChain)
            .setWaitSemaphores(**m_renderFinishedSemaphore[m_currentFramesInFlight]);
        bool needsRecreation = false;
        try {
            auto result = getDevice()->present(&presentInfo);
            if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
                needsRecreation = true;
        } catch (vk::OutOfDateKHRError&) {
            needsRecreation = true;
        }
        if (needsRecreation) {
            m_window->recreateSwapchain();
            return;
        }
    }

#ifndef NDEBUG
    std::cout << std::endl;
#endif

    m_currentFramesInFlight = (m_currentFramesInFlight + 1) % m_framesInFlight;
}

vk::raii::Event* Graph::createEvent(std::function<void()> action)
{
    vk::EventCreateInfo info;
    m_events.push_back(Event());
    m_events.back().vkEvent = std::make_unique<vk::raii::Event>(*getDevice()->getLogicalDevice(), info);
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

Framebuffer& Graph::createFramebuffer()
{
    m_framebuffers.push_back(std::unique_ptr<Framebuffer>(new Framebuffer()));
    m_framebuffers.back()->m_graph = getGraph();
    m_framebuffers.back()->m_device = getDevice();
    return *m_framebuffers.back();
}

GLFWWindow& Graph::getGLFWWindow(uint32_t width, uint32_t height, const std::string& title)
{
    if (!m_window) {
        m_window = std::unique_ptr<GLFWWindow>(new GLFWWindow(width, height, title, this));
        m_window->m_graph = getGraph();
        m_window->m_device = getDevice();
    }
    return *dynamic_cast<GLFWWindow*>(m_window.get());
}

uint32_t Graph::getImageIndex()
{
    return m_window ? m_imageIndex : 0;
}

uint32_t Graph::getFrames()
{
    if (m_window)
        return m_window->m_swapChain->getNumberOfSwapchainFrames();
    return 1;
}

void Graph::createSynchronizationObjects()
{
    vk::FenceCreateInfo fenceCreateInfo;
    fenceCreateInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
    vk::SemaphoreCreateInfo semaphoreCreateInfo;
    if (m_inFlightFence.empty()) {
        for (size_t i = 0; i < m_framesInFlight; i++)
            m_inFlightFence[i] = std::make_unique<vk::raii::Fence>(*getDevice()->getLogicalDevice(), fenceCreateInfo);
    }
    if (m_window && m_imageAvailableSemaphore.empty() && m_renderFinishedSemaphore.empty()) {
        for (size_t i = 0; i < m_framesInFlight; i++) {
            m_imageAvailableSemaphore[i]
                = std::make_unique<vk::raii::Semaphore>(*getDevice()->getLogicalDevice(), semaphoreCreateInfo);
            m_renderFinishedSemaphore[i]
                = std::make_unique<vk::raii::Semaphore>(*getDevice()->getLogicalDevice(), semaphoreCreateInfo);
        }
    }
    if (m_commandBuffers.empty()) {
        for (size_t i = 0; i < m_framesInFlight; i++)
            m_commandBuffers[i].commandPool = getDevice()->getCommandPool();
    }
}
std::vector<vk::raii::CommandBuffer*> Graph::CommandBuffers::getCommandBuffers(size_t count, vk::raii::Device* device)
{
    if (count + usedCommandBuffers > allocatedCommandBuffers.size()) {
        vk::CommandBufferAllocateInfo cmdBufAllocateInfo;
        cmdBufAllocateInfo.setCommandPool(**commandPool)
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(count);
        commandBuffers.push_back(std::make_unique<vk::raii::CommandBuffers>(*device, cmdBufAllocateInfo));
        for (size_t i = 0; i < commandBuffers.back()->size(); i++)
            allocatedCommandBuffers.push_back(&commandBuffers.back()->at(i));
    }
    std::vector<vk::raii::CommandBuffer*> ret;
    ret.insert(ret.end(), allocatedCommandBuffers.begin() + usedCommandBuffers, allocatedCommandBuffers.end());
    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    for (auto& commandBuffer : ret)
        commandBuffer->begin(beginInfo);
    usedCommandBuffers += count;

    return ret;
}

std::vector<vk::CommandBuffer> Graph::CommandBuffers::endCommandBuffers()
{
    std::vector<vk::CommandBuffer> cmdBuffersToSubmit;
    for (size_t i = 0; i < usedCommandBuffers; i++) {
        allocatedCommandBuffers[i]->end();
        cmdBuffersToSubmit.push_back(**allocatedCommandBuffers[i]);
    }
    usedCommandBuffers = 0;
    return cmdBuffersToSubmit;
}

void Graph::CommandBuffers::resetCommandBuffers()
{
    commandPool->reset();
}

std::vector<vk::raii::CommandBuffer*> Graph::getCommandBuffers(size_t count)
{
    return m_commandBuffers[m_currentFramesInFlight].getCommandBuffers(count, getDevice()->getLogicalDevice());
}

void Graph::setNumberOfFramesInFlight(size_t count)
{
    m_framesInFlight = count;
}
