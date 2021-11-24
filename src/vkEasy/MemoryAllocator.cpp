#include <vkEasy/MemoryAllocator.h>

using namespace VK_EASY_NAMESPACE;

MemoryAllocator::MemoryAllocator(const VmaAllocatorCreateInfo& info)
{
    m_allocatorInfo = info;
    vmaCreateAllocator(&m_allocatorInfo, &m_allocator);
}

MemoryAllocator::~MemoryAllocator()
{
    vmaDestroyAllocator(m_allocator);
}

std::unique_ptr<MemoryAllocator::Buffer> MemoryAllocator::createBuffer(
    vk::BufferCreateInfo& pBufferCreateInfo, const VmaAllocationCreateInfo& pAllocationCreateInfo)
{
    return std::unique_ptr<MemoryAllocator::Buffer>(
        new MemoryAllocator::Buffer(*this, pBufferCreateInfo, pAllocationCreateInfo));
}

MemoryAllocator::Buffer::Buffer(MemoryAllocator& allocator, vk::BufferCreateInfo& pBufferCreateInfo,
    const VmaAllocationCreateInfo& pAllocationCreateInfo, VmaAllocationInfo* pAllocationInfo)
{
    m_memoryAllocator = &allocator;
    vmaCreateBuffer(m_memoryAllocator->m_allocator, reinterpret_cast<VkBufferCreateInfo*>(&pBufferCreateInfo),
        &pAllocationCreateInfo, &m_buffer, &m_allocation, pAllocationInfo);
}

MemoryAllocator::Buffer::~Buffer()
{
    vmaDestroyBuffer(m_memoryAllocator->m_allocator, m_buffer, m_allocation);
}

VkBuffer const& MemoryAllocator::Buffer::operator*() const noexcept
{
    return m_buffer;
}

void* MemoryAllocator::Buffer::mapMemory()
{
    void* mapped;
    vmaMapMemory(m_memoryAllocator->m_allocator, m_allocation, &mapped);
    return mapped;
}

void MemoryAllocator::Buffer::unmapMemory()
{
    vmaUnmapMemory(m_memoryAllocator->m_allocator, m_allocation);
}