#include <vkEasy/Context.h>
#include <vkEasy/Graph.h>

using namespace VK_EASY_NAMESPACE;

Graph::Graph()
    : Errorable("Graph")
{
}

void Graph::startBuilding()
{
    m_graph.clear();
    m_building = true;
}

void Graph::stopBuilding()
{
    m_building = false;
    // TODO walk through graph and set features to device
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
        node->execute(m_parent);
}

void Graph::setParent(vk::easy::Device* device)
{
    m_parent = device;
}
