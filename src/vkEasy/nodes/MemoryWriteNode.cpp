#include <iostream>
#include <vkEasy/Graph.h>
#include <vkEasy/nodes/BufferCopyNode.h>
#include <vkEasy/nodes/MemoryWriteNode.h>
#include <vkEasy/resources/StagingBuffer.h>

using namespace VK_EASY_NAMESPACE;

MemoryWriteNode::MemoryWriteNode()
    : Node("MemoryWriteNode")
{
    m_neededQueueTypes = vk::QueueFlagBits::eTransfer;
}

void MemoryWriteNode::update()
{
    if (m_updateData) {
        if (!m_stagingBuffer)
            m_stagingBuffer = &getGraph()->createResource<StagingBuffer>();
        if (!m_bufferCopyNode) {
            m_bufferCopyNode = &getGraph()->createNode<BufferCopyNode>();
            m_bufferCopyNode->setSrcResource(*m_stagingBuffer);
        }
        m_bufferCopyNode->setDstResource(*m_resource, m_offset);
        m_stagingBuffer->setData(*m_data);
        m_bufferCopyNode->execute();
    };
    m_updateData = false;
}

void MemoryWriteNode::setDstResource(Resource& resource)
{
    uses(&resource, Resource::Access::ReadWrite);
    m_resource = &resource;
    auto m_dstBuffer = dynamic_cast<Buffer*>(m_resource);
    if (m_dstBuffer)
        m_dstBuffer->addBufferUsageFlag(vk::BufferUsageFlagBits::eTransferDst);
}

void MemoryWriteNode::setData(const std::vector<uint8_t>& data, size_t offset)
{
    m_updateData = true;
    m_data = &data;
    m_offset = offset;
}