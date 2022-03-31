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
    if (usage == VMA_MEMORY_USAGE_GPU_ONLY)
        m_allocInfo.flags &= ~VMA_ALLOCATION_CREATE_MAPPED_BIT;
    else
        m_allocInfo.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;

    m_allocInfo.usage = usage;
}

vk::DescriptorType Resource::getDescriptorType()
{
    return m_descriptorType;
}

MemoryAllocator::Resource& Resource::getMemory()
{
    return *m_vmaResource.get();
}

void Resource::setGraph(Graph* graph)
{
    m_graph = graph;
    m_device = graph->m_device;
}

void Resource::setOptimization(OptimizationFlags optimization)
{
    if (m_allocInfo.usage == VMA_MEMORY_USAGE_CPU_ONLY || optimization == NO_OPTIMIZATION)
        return;
    setMemoryUsage(static_cast<VmaMemoryUsage>(optimization));
}

bool Resource::exists()
{
    return static_cast<bool>(m_vmaResource);
}

void Resource::update()
{
    if (!exists() || m_recreateResource)
        create();
    m_recreateResource = false;
}

void Resource::destroy()
{
    m_lastAccess.reset();
    m_vmaResource.reset();
}

void Resource::setPersistence(bool persistent)
{
    m_isPersistent = persistent;
}