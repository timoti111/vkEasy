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
    return *m_vmaResource[getActualFrameIndex()].get();
}

void Resource::setOptimization(OptimizationFlags optimization)
{
    if (m_allocInfo.usage == VMA_MEMORY_USAGE_CPU_ONLY || optimization == NO_OPTIMIZATION)
        return;
    setMemoryUsage(static_cast<VmaMemoryUsage>(optimization));
}

bool Resource::exists()
{
    return static_cast<bool>(m_vmaResource[getActualFrameIndex()]);
}

void Resource::update()
{
    if (!exists() || m_recreateResource[getActualFrameIndex()]) {
        m_lastAccess[getActualFrameIndex()].reset();
        create();
    }
    m_recreateResource[getActualFrameIndex()] = false;
}

void Resource::destroy()
{
    m_lastAccess[getActualFrameIndex()].reset();
    m_vmaResource[getActualFrameIndex()].reset();
}

void Resource::setPersistence(bool persistent)
{
    m_isPersistent = persistent;
}

void Resource::setRecreateResource(bool recreate)
{
    for (size_t i = 0; i < m_recreateResource.size(); i++)
        m_recreateResource[i] = recreate;
}

size_t Resource::getActualFrameIndex()
{
    return m_isPersistent ? 0 : getGraph()->getImageIndex();
}

std::optional<Resource::AccessInfo>& Resource::getLastAccess()
{
    return m_lastAccess[getActualFrameIndex()];
}
