#include <iostream>
#include <vkEasy/Device.h>
#include <vkEasy/nodes/BufferCopyNode.h>

using namespace VK_EASY_NAMESPACE;

BufferCopyNode::BufferCopyNode()
    : Node("BufferCopyNode")
{
    m_pipelineStage = vk::PipelineStageFlagBits::eTransfer;
    m_updateFunction = [this](Device* device) {
        if (m_onUpdateFunction)
            m_onUpdateFunction();
        auto transferBuffers = device->getTransferCommandBuffers(1);
        if (transferBuffers.empty())
            return;

        if (m_src->isHostMemory())
            addExecutionBarrier(vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eTransfer);

        std::cout << "Executing: " << objectName() << std::endl;
        if (m_copyRegion.size == VK_WHOLE_SIZE)
            m_copyRegion.setSize(m_src->getSize());
        transferBuffers[0]->copyBuffer(m_src->getVkBuffer(), m_dst->getVkBuffer(), m_copyRegion);

        if (m_dst->isHostMemory())
            addExecutionBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eHost);
    };
}

void BufferCopyNode::setSrcBuffer(Buffer* buffer, size_t size, size_t offset)
{
    uses(buffer);
    m_copyRegion.setSize(size).setSrcOffset(offset);
    m_src = buffer;
    m_src->addBufferUsageFlag(vk::BufferUsageFlagBits::eTransferSrc);
}

void BufferCopyNode::setDstBuffer(Buffer* buffer, size_t offset)
{
    uses(buffer);
    m_copyRegion.setDstOffset(offset);
    m_dst = buffer;
    m_dst->addBufferUsageFlag(vk::BufferUsageFlagBits::eTransferDst);
}

void BufferCopyNode::onUpdate(std::function<void(BufferCopyNode&)> update)
{
    m_onUpdateFunction = [update, this]() { update(*this); };
}