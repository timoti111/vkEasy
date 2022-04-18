#include <vkEasy/Device.h>
#include <vkEasy/nodes/BufferCopyNode.h>
#include <vkEasy/resources/base/Buffer.h>

using namespace VK_EASY_NAMESPACE;

Buffer::Buffer()
    : Resource()
{
    m_isBuffer = true;
    m_bufferCreateInfo.setSharingMode(vk::SharingMode::eExclusive);
    addBufferUsageFlag(vk::BufferUsageFlagBits::eTransferSrc);
    addBufferUsageFlag(vk::BufferUsageFlagBits::eTransferDst);
}

void Buffer::addBufferUsageFlag(vk::BufferUsageFlagBits flag)
{
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
    m_bufferCreateInfo.setSize(size);
}

void Buffer::create()
{
    m_vmaResource[getActualFrameIndex()] = getDevice()->getAllocator()->createBuffer(m_bufferCreateInfo, m_allocInfo);
    m_buffers[getActualFrameIndex()]
        = **dynamic_cast<MemoryAllocator::Buffer*>(m_vmaResource[getActualFrameIndex()].get());
}

void Buffer::setData(const uint8_t* data, size_t size, size_t offset)
{
    setWriteData(data, size, offset);
    setSize(std::max(size + offset, m_bufferCreateInfo.size));
}

void Buffer::transferFromStagingBuffer(StagingBuffer* stagingBuffer, size_t offset)
{
    if (!m_bufferCopyNode)
        m_bufferCopyNode = &getGraph()->createNode<BufferCopyNode>();
    m_bufferCopyNode->setSrcResource(*stagingBuffer);
    m_bufferCopyNode->setDstResource(*this, offset);
    m_bufferCopyNode->execute();
}

void Buffer::transferToStagingBuffer(StagingBuffer* stagingBuffer, size_t offset)
{
    if (!m_bufferCopyNode)
        m_bufferCopyNode = &getGraph()->createNode<BufferCopyNode>();
    m_bufferCopyNode->setSrcResource(*this, stagingBuffer->getSize(), offset);
    m_bufferCopyNode->setDstResource(*stagingBuffer);
    m_bufferCopyNode->execute();
};
