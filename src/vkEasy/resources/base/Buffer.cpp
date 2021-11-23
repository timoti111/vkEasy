#include <iostream>
#include <vkEasy/Device.h>
#include <vkEasy/resources/base/Buffer.h>

using namespace VK_EASY_NAMESPACE;

Buffer::Buffer()
    : Resource()
{
    m_isBuffer = true;
}

void Buffer::addBufferUsageFlag(vk::BufferUsageFlagBits flag)
{
    m_bufferUsageFlags |= flag;
}

vk::raii::Buffer* Buffer::getVkBuffer()
{
    return m_buffer.get();
}

size_t Buffer::getSize()
{
    return m_size;
}

void Buffer::setSize(size_t size)
{
    m_recreateBuffer = size > m_size;
    m_size = size;
}

void Buffer::create()
{
    std::cout << "Creating buffer as: " << vk::to_string(m_bufferUsageFlags) << ""
              << vk::to_string(m_memoryPropertyFlags) << std::endl;

    vk::BufferCreateInfo bufferCreateInfo;
    bufferCreateInfo.setUsage(m_bufferUsageFlags).setSize(m_size).setSharingMode(vk::SharingMode::eExclusive);
    m_buffer = std::make_unique<vk::raii::Buffer>(*m_device->getLogicalDevice(), bufferCreateInfo);

    auto deviceMemoryProperties = m_device->getPhysicalDevice()->getMemoryProperties();
    auto memReqs = m_buffer->getMemoryRequirements();
    vk::MemoryAllocateInfo memAlloc;
    memAlloc.setAllocationSize(memReqs.size);

    bool memTypeFound = false;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memReqs.memoryTypeBits & 1) == 1) {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & m_memoryPropertyFlags)
                == m_memoryPropertyFlags) {
                memAlloc.memoryTypeIndex = i;
                memTypeFound = true;
            }
        }
        memReqs.memoryTypeBits >>= 1;
    }
    assert(memTypeFound);
    m_memory = std::make_unique<vk::raii::DeviceMemory>(*m_device->getLogicalDevice(), memAlloc);

    m_buffer->bindMemory(**m_memory, 0);
}

void Buffer::update()
{
    if (!exists() || m_recreateBuffer)
        create();
    m_recreateBuffer = false;
}
