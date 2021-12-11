#pragma once

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include <vkEasy/global.h>
#include <vk_mem_alloc.h>

namespace VK_EASY_NAMESPACE {

class MemoryAllocator {

public:
    MemoryAllocator(const VmaAllocatorCreateInfo& info, vk::raii::Device* device, vk::raii::Instance* instance);
    ~MemoryAllocator();
    MemoryAllocator() = delete;
    MemoryAllocator(MemoryAllocator const&) = delete;
    void operator=(MemoryAllocator const&) = delete;

    class Resource {
        friend class MemoryAllocator;

    public:
        virtual ~Resource() = default;
        Resource() = delete;
        Resource(Resource const&) = delete;
        void operator=(Resource const&) = delete;

        size_t getSize();
        bool isMappable();
        void* mapMemory();
        void unmapMemory();

    protected:
        Resource(MemoryAllocator& allocator);
        void updateMemFlags();
        size_t m_size;
        VmaAllocation m_allocation;
        VmaAllocationInfo m_allocationInfo;
        MemoryAllocator* m_memoryAllocator;
        bool m_isPersistent = false;

    private:
        void* m_mapped;
        VkMemoryPropertyFlags m_memFlags;
    };

    class Buffer : public MemoryAllocator::Resource {
        friend class MemoryAllocator;

    public:
        ~Buffer();
        Buffer() = delete;
        Buffer(Buffer const&) = delete;
        void operator=(Buffer const&) = delete;

        VkBuffer const& operator*() const noexcept;

    private:
        Buffer(MemoryAllocator& allocator, vk::BufferCreateInfo& bufferCreateInfo,
            const VmaAllocationCreateInfo& allocationCreateInfo);
        VkBuffer m_buffer;
    };
    friend class Buffer;
    std::unique_ptr<Buffer> createBuffer(
        vk::BufferCreateInfo& bufferCreateInfo, const VmaAllocationCreateInfo& allocationCreateInfo);

    class Image : public MemoryAllocator::Resource {
        friend class MemoryAllocator;

    public:
        ~Image();
        Image() = delete;
        Image(Image const&) = delete;
        void operator=(Image const&) = delete;

        VkImage const& operator*() const noexcept;

    private:
        Image(MemoryAllocator& allocator, vk::ImageCreateInfo& bufferCreateInfo,
            const VmaAllocationCreateInfo& allocationCreateInfo);
        VkImage m_image;
    };
    friend class Image;
    std::unique_ptr<Image> createImage(
        vk::ImageCreateInfo& imageCreateInfo, const VmaAllocationCreateInfo& allocationCreateInfo);

private:
    VmaAllocatorCreateInfo m_allocatorInfo;
    VmaVulkanFunctions m_vulkanFunctions;
    VmaAllocator m_allocator;
};
} // namespace VK_EASY_NAMESPACE
