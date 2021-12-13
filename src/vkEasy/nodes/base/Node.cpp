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

void Node::addExecutionBarrier(vk::PipelineStageFlagBits src, vk::PipelineStageFlagBits dst)
{
    if (dst == vk::PipelineStageFlagBits::eNoneKHR)
        return;
    m_graph->setActualPipelineStage(dst);
    if (src == vk::PipelineStageFlagBits::eNoneKHR)
        return;

#ifndef NDEBUG
    std ::cout << "Adding execution barrier between { " << vk::to_string(src) << " } and { " << vk::to_string(dst)
               << " }" << std::endl;
#endif
    auto buffers = m_device->getUniversalCommandBuffers(1);
    if (buffers.empty())
        return;
    buffers[0]->pipelineBarrier(src, dst, {}, {}, {}, {});
}

void Node::addExecutionBarrier(vk::PipelineStageFlagBits dst)
{
    addExecutionBarrier(m_graph->getLastPipelineStage(), dst);
}

void Node::addBufferBarrier(vk::PipelineStageFlags src, vk::PipelineStageFlags dst, vk::Buffer buffer,
    vk::AccessFlagBits srcMask, vk::AccessFlagBits dstMask)
{
#ifndef NDEBUG
    std ::cout << "Adding execution barrier between { " << vk::to_string(src) << " } and { " << vk::to_string(dst)
               << " }" << std::endl;
#endif
    vk::BufferMemoryBarrier bufferBarrier;
    bufferBarrier.setBuffer(buffer)
        .setSize(VK_WHOLE_SIZE)
        .setSrcAccessMask(srcMask)
        .setDstAccessMask(dstMask)
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
    auto buffers = m_device->getUniversalCommandBuffers(1);
    if (buffers.empty())
        return;
    buffers[0]->pipelineBarrier(src, dst, {}, {}, bufferBarrier, {});
}

void Node::addEvent(std::function<void()> event)
{
#ifndef NDEBUG
    std::cout << "Adding event after { " << vk::to_string(m_graph->getLastPipelineStage()) << " }" << std::endl;
#endif
    auto vkEvent = m_graph->createEvent(event);
    auto buffers = m_device->getUniversalCommandBuffers(1);
    if (buffers.empty())
        return;
    buffers[0]->setEvent(**vkEvent, vk::PipelineStageFlagBits::eAllCommands);
}

void Node::execute()
{
    addExecutionBarrier(m_pipelineStage);
    for (auto& usedResource : m_usedResources)
        usedResource->update();
#ifndef NDEBUG
    std::cout << "Executing { " << objectName() << " }" << std::endl;
#endif
    update(m_device);
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