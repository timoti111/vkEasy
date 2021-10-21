#include <vkEasy/Device.h>
#include <vkEasy/Graph.h>
#include <vkEasy/nodes/base/Node.h>

using namespace VK_EASY_NAMESPACE;

Node::Node(const std::string& nodeName)
    : Errorable(nodeName)
{
}

void Node::readsFrom(Resource* resource, uint32_t binding)
{
    addToGraph();
}

void Node::writesTo(Resource* resource, uint32_t binding)
{
    addToGraph();
}

void Node::execute(Device* device)
{
    if (m_updateFunction)
        m_updateFunction(device);
}

void Node::addToGraph()
{
    m_parent->addToGraph(this);
}

Graph* Node::getParent()
{
    return m_parent;
}

void Node::setParent(Graph* parent)
{
    m_parent = parent;
}

void Node::needsExtensions(const vk::ArrayProxyNoTemporaries<std::string>& extensions)
{
}