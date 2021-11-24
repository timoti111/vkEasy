#include <vkEasy/Device.h>
#include <vkEasy/Graph.h>
#include <vkEasy/resources/base/Resource.h>

using namespace VK_EASY_NAMESPACE;

bool Resource::isBuffer()
{
    return m_isBuffer;
}

void Resource::setMemoryUsage(VmaMemoryUsage usage)
{
    m_allocInfo.usage = usage;
    m_isHostMemory = usage == VMA_MEMORY_USAGE_GPU_TO_CPU || usage == VMA_MEMORY_USAGE_CPU_TO_GPU
        || usage == VMA_MEMORY_USAGE_CPU_ONLY;
}

bool Resource::isHostMemory()
{
    return m_isHostMemory;
}

vk::DescriptorType Resource::getDescriptorType()
{
    return m_descriptorType;
}

void Resource::setGraph(Graph* graph)
{
    m_graph = graph;
    m_device = graph->m_device;
}