#include <vkEasy/nodes/ResourceWriteNode.h>

using namespace VK_EASY_NAMESPACE;

ResourceWriteNode::ResourceWriteNode()
    : ResourceOperationNode("ResourceWriteNode")
{
}

void ResourceWriteNode::onUpdate(std::function<void(ResourceWriteNode&)> updateFunction)
{
}

void ResourceWriteNode::setData(const std::vector<uint32_t>& data, size_t offset)
{
}