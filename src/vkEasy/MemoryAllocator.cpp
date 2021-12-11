#include <vkEasy/MemoryAllocator.h>

using namespace VK_EASY_NAMESPACE;

MemoryAllocator::MemoryAllocator(
    const VmaAllocatorCreateInfo& info, vk::raii::Device* device, vk::raii::Instance* instance)
{
    m_vulkanFunctions = VmaVulkanFunctions();
    m_vulkanFunctions.vkAllocateMemory = device->getDispatcher()->vkAllocateMemory;
    m_vulkanFunctions.vkBindBufferMemory2KHR = device->getDispatcher()->vkBindBufferMemory2KHR;
    m_vulkanFunctions.vkBindBufferMemory = device->getDispatcher()->vkBindBufferMemory;
    m_vulkanFunctions.vkBindImageMemory2KHR = device->getDispatcher()->vkBindImageMemory2KHR;
    m_vulkanFunctions.vkBindImageMemory = device->getDispatcher()->vkBindImageMemory;
    m_vulkanFunctions.vkCmdCopyBuffer = device->getDispatcher()->vkCmdCopyBuffer;
    m_vulkanFunctions.vkCreateBuffer = device->getDispatcher()->vkCreateBuffer;
    m_vulkanFunctions.vkCreateImage = device->getDispatcher()->vkCreateImage;
    m_vulkanFunctions.vkDestroyBuffer = device->getDispatcher()->vkDestroyBuffer;
    m_vulkanFunctions.vkDestroyImage = device->getDispatcher()->vkDestroyImage;
    m_vulkanFunctions.vkFlushMappedMemoryRanges = device->getDispatcher()->vkFlushMappedMemoryRanges;
    m_vulkanFunctions.vkFreeMemory = device->getDispatcher()->vkFreeMemory;
    m_vulkanFunctions.vkGetBufferMemoryRequirements2KHR = device->getDispatcher()->vkGetBufferMemoryRequirements2KHR;
    m_vulkanFunctions.vkGetBufferMemoryRequirements = device->getDispatcher()->vkGetBufferMemoryRequirements;
    m_vulkanFunctions.vkGetImageMemoryRequirements2KHR = device->getDispatcher()->vkGetImageMemoryRequirements2KHR;
    m_vulkanFunctions.vkGetImageMemoryRequirements = device->getDispatcher()->vkGetImageMemoryRequirements;
    m_vulkanFunctions.vkGetPhysicalDeviceMemoryProperties2KHR
        = instance->getDispatcher()->vkGetPhysicalDeviceMemoryProperties2KHR;
    m_vulkanFunctions.vkGetPhysicalDeviceMemoryProperties
        = instance->getDispatcher()->vkGetPhysicalDeviceMemoryProperties;
    m_vulkanFunctions.vkGetPhysicalDeviceProperties = instance->getDispatcher()->vkGetPhysicalDeviceProperties;
    m_vulkanFunctions.vkInvalidateMappedMemoryRanges = device->getDispatcher()->vkInvalidateMappedMemoryRanges;
    m_vulkanFunctions.vkMapMemory = device->getDispatcher()->vkMapMemory;
    m_vulkanFunctions.vkUnmapMemory = device->getDispatcher()->vkUnmapMemory;

    m_allocatorInfo = info;
    m_allocatorInfo.pVulkanFunctions = &m_vulkanFunctions;
    vmaCreateAllocator(&m_allocatorInfo, &m_allocator);
}

MemoryAllocator::~MemoryAllocator()
{
    vmaDestroyAllocator(m_allocator);
}

MemoryAllocator::Resource::Resource(MemoryAllocator& allocator)
    : m_memoryAllocator(&allocator)
    , m_mapped(nullptr)
{
}

size_t MemoryAllocator::Resource::getSize()
{
    return m_size;
}

bool MemoryAllocator::Resource::isMappable()
{
    return (m_memFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0;
}
void* MemoryAllocator::Resource::mapMemory()
{
    if (!isMappable())
        return nullptr;
    vmaInvalidateAllocation(m_memoryAllocator->m_allocator, m_allocation, 0, VK_WHOLE_SIZE);
    if (!m_isPersistent)
        vmaMapMemory(m_memoryAllocator->m_allocator, m_allocation, &m_mapped);
    return m_mapped;
}

void MemoryAllocator::Resource::unmapMemory()
{
    if (m_mapped == nullptr)
        return;
    vmaFlushAllocation(m_memoryAllocator->m_allocator, m_allocation, 0, VK_WHOLE_SIZE);
    if (!m_isPersistent) {
        vmaUnmapMemory(m_memoryAllocator->m_allocator, m_allocation);
        m_mapped = nullptr;
    }
}

void MemoryAllocator::Resource::updateMemFlags()
{
    vmaGetAllocationMemoryProperties(m_memoryAllocator->m_allocator, m_allocation, &m_memFlags);
    if (m_allocationInfo.pMappedData) {
        m_mapped = m_allocationInfo.pMappedData;
        m_isPersistent = true;
    }
}

std::unique_ptr<MemoryAllocator::Buffer> MemoryAllocator::createBuffer(
    vk::BufferCreateInfo& bufferCreateInfo, const VmaAllocationCreateInfo& allocationCreateInfo)
{
    return std::unique_ptr<MemoryAllocator::Buffer>(
        new MemoryAllocator::Buffer(*this, bufferCreateInfo, allocationCreateInfo));
}

MemoryAllocator::Buffer::Buffer(MemoryAllocator& allocator, vk::BufferCreateInfo& bufferCreateInfo,
    const VmaAllocationCreateInfo& allocationCreateInfo)
    : MemoryAllocator::Resource(allocator)
{
    m_size = bufferCreateInfo.size;
    vmaCreateBuffer(m_memoryAllocator->m_allocator, reinterpret_cast<VkBufferCreateInfo*>(&bufferCreateInfo),
        &allocationCreateInfo, &m_buffer, &m_allocation, &m_allocationInfo);
    updateMemFlags();
}

MemoryAllocator::Buffer::~Buffer()
{
    vmaDestroyBuffer(m_memoryAllocator->m_allocator, m_buffer, m_allocation);
}

VkBuffer const& MemoryAllocator::Buffer::operator*() const noexcept
{
    return m_buffer;
}

std::unique_ptr<MemoryAllocator::Image> MemoryAllocator::createImage(
    vk::ImageCreateInfo& imageCreateInfo, const VmaAllocationCreateInfo& allocationCreateInfo)
{
    return std::unique_ptr<MemoryAllocator::Image>(
        new MemoryAllocator::Image(*this, imageCreateInfo, allocationCreateInfo));
}

MemoryAllocator::Image::Image(MemoryAllocator& allocator, vk::ImageCreateInfo& imageCreateInfo,
    const VmaAllocationCreateInfo& allocationCreateInfo)
    : MemoryAllocator::Resource(allocator)
{
    vmaCreateImage(m_memoryAllocator->m_allocator, reinterpret_cast<VkImageCreateInfo*>(&imageCreateInfo),
        &allocationCreateInfo, &m_image, &m_allocation, &m_allocationInfo);
    m_size = m_allocationInfo.size;
    updateMemFlags();
}

MemoryAllocator::Image::~Image()
{
    vmaDestroyImage(m_memoryAllocator->m_allocator, m_image, m_allocation);
}

VkImage const& MemoryAllocator::Image::operator*() const noexcept
{
    return m_image;
}