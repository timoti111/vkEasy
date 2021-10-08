#include <vkEasy/nodes/ResourceReadNode.h>

using namespace VK_EASY_NAMESPACE;

void ResourceReadNode::onDataReady(std::function<void(const std::vector<uint32_t>&)> dataReady)
{
}

void ResourceReadNode::onUpdate(std::function<void(ResourceReadNode&)> update)
{
}

void ResourceReadNode::readsFrom(const vk::ArrayProxyNoTemporaries<Resource>& resources)
{
}

void ResourceReadNode::setDataToRead(size_t offset, size_t size)
{
}