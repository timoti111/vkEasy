#include <vkEasy/Device.h>
#include <vkEasy/Graph.h>
#include <vkEasy/resources/base/Resource.h>

using namespace VK_EASY_NAMESPACE;

bool Resource::isBuffer()
{
    return m_isBuffer;
}

void Resource::addMemoryPropertyFlag(vk::MemoryPropertyFlagBits flag)
{
    m_memoryPropertyFlags |= flag;
    if (flag == vk::MemoryPropertyFlagBits::eHostCached || flag == vk::MemoryPropertyFlagBits::eHostCoherent
        || flag == vk::MemoryPropertyFlagBits::eHostVisible)
        m_isHostMemory = true;
}

bool Resource::isHostMemory()
{
    return m_isHostMemory;
}

vk::DescriptorType Resource::getDescriptorType()
{
    return m_descriptorType;
}

bool Resource::exists()
{
    return static_cast<bool>(m_memory);
}

void Resource::setGraph(Graph* graph)
{
    m_graph = graph;
    m_device = graph->m_device;
}