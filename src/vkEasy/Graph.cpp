#include <vkEasy/Context.h>
#include <vkEasy/Graph.h>

using namespace VK_EASY_NAMESPACE;

Graph::Graph()
    : Errorable("Graph")
{
}

void Graph::setPhysicalDevice(vk::raii::PhysicalDevice* physicalDevice)
{
    m_physicalDevice = physicalDevice;
}

void Graph::startBuilding()
{
    m_graph.clear();
    m_building = true;
}

void Graph::stopBuilding()
{
    m_building = false;
    // TODO walk through graph, choose device etc
}

void Graph::addToGraph(Node* node)
{
    if (!m_building)
        return;
    if (std::find(m_graph.begin(), m_graph.end(), node) == m_graph.end())
        m_graph.push_back(node);
}

void Graph::run()
{
    if (m_building)
        return;

    for (auto& node : m_graph)
        node->execute();
}

void Graph::setParent(Context* context)
{
    m_parent = context;
}
