#include <vkEasy/nodes/PresentNode.h>

using namespace VK_EASY_NAMESPACE;

PresentNode::PresentNode()
    : Node("PresentNode")
{
    m_neededQueueTypes = vk::QueueFlagBits::eGraphics;
}

void PresentNode::update(Device* device)
{
}
