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

VkBuffer Buffer::getVkBuffer()
{
    return **m_buffer;
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
    vk::BufferCreateInfo bufferCreateInfo;
    bufferCreateInfo.setUsage(m_bufferUsageFlags).setSize(m_size).setSharingMode(vk::SharingMode::eExclusive);
    m_buffer = m_device->getAllocator()->createBuffer(bufferCreateInfo, m_allocInfo);
}

void Buffer::update()
{
    if (!exists() || m_recreateBuffer)
        create();
    m_recreateBuffer = false;
}

bool Buffer::exists()
{
    return static_cast<bool>(m_buffer);
}