#include <algorithm>
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
    for (auto& node : m_nodeOrderGraph) {
        for (auto& resource : node->m_usedResources) {
            if (resourceUsageCopy[resource].size() == m_resourceUsage[resource].size())
                m_callGraph.push_back([=]() { resource->create(m_device); });
        };
        m_callGraph.push_back([=]() { node->execute(); });
        for (auto& resource : node->m_usedResources) {
            auto& nodes = resourceUsageCopy[resource];
            nodes.erase(std::remove(nodes.begin(), nodes.end(), node), nodes.end());
            if (resourceUsageCopy[resource].size() == 0)
                m_callGraph.push_back([=]() { resource->destroy(m_device); });
        };
    }
    m_device->initialize();
}

void Graph::run()
{
    if (m_recording)
        error(Error::RecordingGraph);

    for (auto& action : m_callGraph)
        action();

    m_device->sendCommandBuffers();
    m_device->waitForFences();
    m_device->resetCommandBuffers();
}

void Graph::setDevice(Device* device)
{
    if (m_device == device)
        return;
    m_device = device;
    for (auto& node : m_nodes)
        node->setGraph(this);
}
