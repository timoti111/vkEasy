#include <vkEasy/nodes/base/ResourceOperationNode.h>

using namespace VK_EASY_NAMESPACE;

ResourceOperationNode::ResourceOperationNode(const std::string& nodeName)
    : Node(nodeName)
{
}

void ResourceOperationNode::readsFrom(Resource* resource)
{
}

void ResourceOperationNode::writesTo(Resource* resource)
{
}