#include <iostream>
#include <vkEasy/Device.h>
#include <vkEasy/Graph.h>
#include <vkEasy/nodes/base/Node.h>

using namespace VK_EASY_NAMESPACE;

Node::Node(const std::string& nodeName)
    : Errorable(nodeName)
{
}

void Node::uses(Resource* resource)
{
    m_usedResources.emplace(resource);
}

void Node::operator()()
{
    if (!m_graph->m_recording)
        error(Error::NotRecordingGraph);
    m_dependantNodes.clear();
    if (!m_graph->m_nodeOrderGraph.empty()) {
        m_dependantNodes.push_back(m_graph->m_nodeOrderGraph.back());
        m_graph->m_nodeOrderGraph.back()->setNext(this);
    }
    m_graph->m_nodeOrderGraph.push_back(this);

    for (auto& usedResource : m_usedResources)
        m_graph->m_resourceUsage[usedResource].push_back(this);
}

void Node::addExecutionBarrier(vk::PipelineStageFlags src, vk::PipelineStageFlags dst)
{
    std::cout << "Execution barrier from: " << vk::to_string(src) << " to: " << vk::to_string(dst) << std::endl;
    auto buffers = m_device->getComputeCommandBuffers(1);
    if (buffers.empty())
        return;
    buffers[0]->pipelineBarrier(src, dst, {}, {}, {}, {});
}

void Node::addBufferBarrier(vk::PipelineStageFlags src, vk::PipelineStageFlags dst, vk::Buffer buffer,
    vk::AccessFlagBits srcMask, vk::AccessFlagBits dstMask)
{
    vk::BufferMemoryBarrier bufferBarrier;
    bufferBarrier.setBuffer(buffer)
        .setSize(VK_WHOLE_SIZE)
        .setSrcAccessMask(srcMask)
        .setDstAccessMask(dstMask)
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
    std::cout << "Buffer barrier from: " << vk::to_string(src) << " to: " << vk::to_string(dst) << std::endl;
    auto buffers = m_device->getComputeCommandBuffers(1);
    if (buffers.empty())
        return;
    buffers[0]->pipelineBarrier(src, dst, {}, {}, bufferBarrier, {});
}

void Node::addEvent(std::function<void()> event)
{
    std::cout << "Adding event: " << vk::to_string(m_graph->getLastPipelineStage()) << std::endl;
    auto vkEvent = m_graph->createEvent(event);
    auto buffers = m_device->getComputeCommandBuffers(1);
    if (buffers.empty())
        return;
    buffers[0]->setEvent(**vkEvent, m_graph->getLastPipelineStage());
}

void Node::execute()
{
    if (m_pipelineStage != vk::PipelineStageFlagBits::eNoneKHR) {
        auto lastPipelineStage = m_graph->getLastPipelineStage();
        m_graph->setActualPipelineStage(m_pipelineStage);
        if (lastPipelineStage != vk::PipelineStageFlagBits::eNoneKHR)
            addExecutionBarrier(lastPipelineStage, m_pipelineStage);
    }

    for (auto& usedResource : m_usedResources)
        usedResource->update();
    if (m_preUpdateFunction)
        m_preUpdateFunction();
    std::cout << "Executing: " << objectName() << std::endl;
    update(m_device);
    if (m_postUpdateFunction)
        m_postUpdateFunction();
}

Graph* Node::getGraph()
{
    return m_graph;
}

Device* Node::getDevice()
{
    return m_graph->m_device;
}

void Node::setGraph(Graph* graph)
{
    m_graph = graph;
    m_device = graph->m_device;
}

void Node::setNext(Node* next)
{
    m_nextNode = next;
}

void Node::needsExtensions(const std::initializer_list<std::string>& extensions)
{
    for (auto& extension : extensions)
        m_neededExtensions.emplace(extension);
}