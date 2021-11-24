#pragma once

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include <vkEasy/global.h>
#include <vk_mem_alloc.h>

namespace VK_EASY_NAMESPACE {

class MemoryAllocator {

public:
    MemoryAllocator(const VmaAllocatorCreateInfo& info);
    ~MemoryAllocator();
    MemoryAllocator() = delete;
    MemoryAllocator(MemoryAllocator const&) = delete;
    void operator=(MemoryAllocator const&) = delete;

    class Buffer {
        friend class MemoryAllocator;

    public:
        ~Buffer();
        Buffer() = delete;
        Buffer(Buffer const&) = delete;
        void operator=(Buffer const&) = delete;

        VkBuffer const& operator*() const noexcept;
        void* mapMemory();
        void unmapMemory();

    private:
        Buffer(MemoryAllocator& allocator, vk::BufferCreateInfo& pBufferCreateInfo,
            const VmaAllocationCreateInfo& pAllocationCreateInfo, VmaAllocationInfo* pAllocationInfo = nullptr);
        VkBuffer m_buffer;
        VmaAllocation m_allocation;
        MemoryAllocator* m_memoryAllocator;
    };
    friend class Buffer;

    std::unique_ptr<Buffer> createBuffer(
        vk::BufferCreateInfo& pBufferCreateInfo, const VmaAllocationCreateInfo& pAllocationCreateInfo);

private:
    VmaAllocatorCreateInfo m_allocatorInfo;
    VmaAllocator m_allocator;
};
} // namespace VK_EASY_NAMESPACE
