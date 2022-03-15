#include <vkEasy/Device.h>
#include <vkEasy/resources/base/Buffer.h>

using namespace VK_EASY_NAMESPACE;

Buffer::Buffer()
    : Resource()
{
    m_isBuffer = true;
    m_bufferCreateInfo.setSharingMode(vk::SharingMode::eExclusive);
}

void Buffer::addBufferUsageFlag(vk::BufferUsageFlagBits flag)
{
    m_recreateResource = true;
    m_bufferCreateInfo.setUsage(m_bufferCreateInfo.usage | flag);
}

VkBuffer Buffer::getVkBuffer()
{
    return **dynamic_cast<MemoryAllocator::Buffer*>(m_vmaResource.get());
}

size_t Buffer::getSize()
{
    return m_bufferCreateInfo.size;
}

void Buffer::setSize(size_t size)
{
    m_recreateResource = true;
    m_bufferCreateInfo.setSize(size);
}

void Buffer::create()
{
    m_vmaResource = m_device->getAllocator()->createBuffer(m_bufferCreateInfo, m_allocInfo);
}