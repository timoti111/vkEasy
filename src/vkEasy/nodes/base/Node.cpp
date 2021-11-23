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
    auto transferBuffers = m_device->getTransferCommandBuffers(1);
    if (transferBuffers.empty())
        return;
    transferBuffers[0]->pipelineBarrier(src, dst, {}, {}, {}, {});
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
    auto transferBuffers = m_device->getTransferCommandBuffers(1);
    if (transferBuffers.empty())
        return;
    transferBuffers[0]->pipelineBarrier(src, dst, {}, {}, bufferBarrier, {});
}

void Node::execute()
{
    if (!m_dependantNodes.empty())
        addExecutionBarrier(m_dependantNodes[0]->m_pipelineStage, m_pipelineStage);
    for (auto& usedResource : m_usedResources)
        usedResource->update();
    if (m_updateFunction)
        m_updateFunction(m_device);
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