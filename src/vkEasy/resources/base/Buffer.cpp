#include <iostream>
#include <vkEasy/Context.h>
#include <vkEasy/Device.h>
#include <vkEasy/nodes/MemoryCopyNode.h>
#include <vkEasy/resources/base/Buffer.h>

using namespace VK_EASY_NAMESPACE;

Buffer::Buffer()
    : Resource()
{
    m_isBuffer = true;
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
VkBuffer Buffer::getVkBuffer(size_t index)
{
    return m_buffers[m_isPersistent ? 0 : index];
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
    if (m_queueIndicesVector.size() > 1) {
        m_bufferCreateInfo.setQueueFamilyIndices(m_queueIndicesVector);
        m_bufferCreateInfo.setSharingMode(vk::SharingMode::eConcurrent);
    }
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
        m_bufferCopyNode = &getGraph()->createNode<MemoryCopyNode>();
    m_bufferCopyNode->setSrcBuffer(*stagingBuffer);
    m_bufferCopyNode->setDstBuffer(*this, offset);
    m_bufferCopyNode->execute();
}

void Buffer::transferToStagingBuffer(StagingBuffer* stagingBuffer, size_t offset)
{
    if (!m_bufferCopyNode)
        m_bufferCopyNode = &getGraph()->createNode<MemoryCopyNode>();
    m_bufferCopyNode->setSrcBuffer(*this, stagingBuffer->getSize(), offset);
    m_bufferCopyNode->setDstBuffer(*stagingBuffer);
    m_bufferCopyNode->execute();
};

void Buffer::solveSynchronization(vk::PipelineStageFlagBits stage, Access access)
{
    auto& lastAccess = getLastAccessInfo();
    auto nextStage = stage;
    auto nextAccess = access == Access::ReadOnly ? vk::AccessFlagBits::eMemoryRead : vk::AccessFlagBits::eMemoryWrite;

    vk::BufferMemoryBarrier bufferBarrier;
    bufferBarrier.setBuffer(getVkBuffer())
        .setSize(VK_WHOLE_SIZE)
        .setSrcAccessMask(lastAccess.lastAccess ? lastAccess.lastAccess.value() : vk::AccessFlagBits::eNone)
        .setDstAccessMask(nextAccess)
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);

    if (lastAccess.lastRead && access == Access::ReadWrite)
        insertBufferBarrier(lastAccess.lastRead.value(), nextStage, bufferBarrier);
    if (lastAccess.lastWrite && access == Access::ReadWrite)
        insertBufferBarrier(lastAccess.lastWrite.value(), nextStage, bufferBarrier);

    if (access == Access::ReadOnly)
        lastAccess.lastRead = nextStage;
    if (access == Access::ReadWrite)
        lastAccess.lastWrite = nextStage;
    lastAccess.lastAccess = nextAccess;
}

void Buffer::insertBufferBarrier(
    vk::PipelineStageFlagBits src, vk::PipelineStageFlagBits dst, const vk::BufferMemoryBarrier& barrier)
{
    auto buffers = getGraph()->getCommandBuffers(1);
    if (buffers.empty())
        return;
    if (Context::get().getDebugOutput())
        std ::cout << "Adding buffer barrier between { " << vk::to_string(src) << " } and { " << vk::to_string(dst)
                   << " }" << std::endl;
    buffers[0]->pipelineBarrier(src, dst, {}, {}, barrier, {});
}
