#include <vkEasy/nodes/base/ResourceOperationNode.h>

using namespace VK_EASY_NAMESPACE;

ResourceOperationNode::ResourceOperationNode(const std::string& nodeName)
    : Node(nodeName)
{
}

void ResourceOperationNode::readsFrom(Resource* resource)
{
    Node::readsFrom(resource, 0);
}

void ResourceOperationNode::writesTo(Resource* resource)
{
    Node::writesTo(resource, 0);
}