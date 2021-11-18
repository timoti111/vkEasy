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

void Node::execute()
{
    if (m_updateFunction)
        m_updateFunction(m_device);
    auto cmdBuffers = m_device->getCommandBuffers(5, vk::QueueFlagBits::eCompute);
}

Graph* Node::getGraph()
{
    return m_graph;
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