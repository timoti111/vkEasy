#include <cmath>
#include <iostream>
#include <vkEasy/Device.h>
#include <vkEasy/nodes/BufferCopyNode.h>
#include <vkEasy/nodes/MemoryReadNode.h>
#include <vkEasy/resources/StagingBuffer.h>

using namespace VK_EASY_NAMESPACE;

MemoryReadNode::MemoryReadNode()
    : Node("MemoryReadNode")
{
    setDataToRead();
    m_neededQueueTypes = vk::QueueFlagBits::eTransfer;
    m_pipelineStage = vk::PipelineStageFlagBits::eNoneKHR;
}

void MemoryReadNode::update(Device* device)
{
    auto bytesToRead = std::min(m_size, m_resource->getMemory().getSize());
    m_data.resize(bytesToRead);
    if (!m_resource->getMemory().isMappable()) {
        if (!m_stagingBuffer)
            m_stagingBuffer = &getGraph()->createResource<StagingBuffer>();
        if (!m_bufferCopyNode) {
            m_bufferCopyNode = &getGraph()->createNode<BufferCopyNode>();
            m_bufferCopyNode->setDstResource(*m_stagingBuffer);
        }
        m_stagingBuffer->setSize(bytesToRead);
        m_bufferCopyNode->setSrcResource(*m_resource, bytesToRead, m_offset);
        m_bufferCopyNode->execute();
    }
    addEvent([this]() {
        if (m_onDataReady)
            m_onDataReady(getData());
    });
    addExecutionBarrier(vk::PipelineStageFlagBits::eHost);
}

void MemoryReadNode::setSrcResource(Resource& resource)
{
    uses(&resource);
    m_resource = &resource;
    auto m_srcBuffer = dynamic_cast<Buffer*>(m_resource);
    if (m_srcBuffer)
        m_srcBuffer->addBufferUsageFlag(vk::BufferUsageFlagBits::eTransferSrc);
}

void MemoryReadNode::setDataToRead(size_t offset, size_t size)
{
    m_offset = offset;
    m_size = size;
}
void MemoryReadNode::onDataReady(std::function<void(const std::vector<uint8_t>&)> dataReady)
{
    m_onDataReady = dataReady;
}

const std::vector<uint8_t>& MemoryReadNode::getData()
{
    if (m_stagingBuffer)
        m_stagingBuffer->getData(m_data);
    else {
        auto memoryPtr = reinterpret_cast<uint8_t*>(m_resource->getMemory().mapMemory());
        memcpy(m_data.data(), memoryPtr + m_offset, m_data.size());
        m_resource->getMemory().unmapMemory();
    }
    return m_data;
}
