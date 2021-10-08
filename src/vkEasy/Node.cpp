#include <vkEasy/Node.h>

using namespace VK_EASY_NAMESPACE;

Node::Node()
{
}

void Node::prepare()
{
}

void Node::execute()
{
}

void Node::needsExtensions(const vk::ArrayProxyNoTemporaries<Resource>& extensions)
{
}

void Node::skipIf(std::function<bool()> predicate)
{
}