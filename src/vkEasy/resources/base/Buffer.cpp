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
    setRecreateResource(true);
    m_bufferCreateInfo.setUsage(m_bufferCreateInfo.usage | flag);
}

VkBuffer Buffer::getVkBuffer()
{
    return m_buffers[getActualFrameIndex()];
}

size_t Buffer::getSize()
{
    return m_bufferCreateInfo.size;
}

void Buffer::setSize(size_t size)
{
    setRecreateResource(true);
    m_bufferCreateInfo.setSize(size);
}

void Buffer::create()
{
    m_vmaResource[getActualFrameIndex()] = getDevice()->getAllocator()->createBuffer(m_bufferCreateInfo, m_allocInfo);
    m_buffers[getActualFrameIndex()]
        = **dynamic_cast<MemoryAllocator::Buffer*>(m_vmaResource[getActualFrameIndex()].get());
}