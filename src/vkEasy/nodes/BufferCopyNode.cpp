#include <iostream>
#include <vkEasy/Device.h>
#include <vkEasy/nodes/BufferCopyNode.h>
#include <vkEasy/resources/base/Buffer.h>

using namespace VK_EASY_NAMESPACE;

BufferCopyNode::BufferCopyNode()
    : Node("BufferCopyNode")
{
    m_neededQueueTypes = vk::QueueFlagBits::eTransfer;
    m_pipelineStage = vk::PipelineStageFlagBits::eTransfer;
}

void BufferCopyNode::update()
{
    auto transferBuffers = getCommandBuffers(1);
    if (transferBuffers.empty())
        return;

    auto m_srcBuffer = dynamic_cast<Buffer*>(m_src);
    auto m_dstBuffer = dynamic_cast<Buffer*>(m_dst);
    if (m_srcBuffer && m_dstBuffer) {
        if (m_copyRegion.size == VK_WHOLE_SIZE)
            m_copyRegion.setSize(m_srcBuffer->getSize() - m_copyRegion.srcOffset);
        transferBuffers[0]->copyBuffer(m_srcBuffer->getVkBuffer(), m_dstBuffer->getVkBuffer(), m_copyRegion);
    }
}

void BufferCopyNode::setSrcResource(Resource& resource, size_t size, size_t offset)
{
    uses(&resource, Resource::Access::ReadOnly);
    m_src = &resource;
    auto m_srcBuffer = dynamic_cast<Buffer*>(m_src);
    if (m_srcBuffer) {
        m_copyRegion.setSize(size).setSrcOffset(offset);
        m_srcBuffer->addBufferUsageFlag(vk::BufferUsageFlagBits::eTransferSrc);
    }
}

void BufferCopyNode::setDstResource(Resource& resource, size_t offset)
{
    uses(&resource, Resource::Access::ReadWrite);
    m_dst = &resource;
    auto m_dstBuffer = dynamic_cast<Buffer*>(m_dst);
    if (m_dstBuffer) {
        m_copyRegion.setDstOffset(offset);
        m_dstBuffer->addBufferUsageFlag(vk::BufferUsageFlagBits::eTransferDst);
    }
}