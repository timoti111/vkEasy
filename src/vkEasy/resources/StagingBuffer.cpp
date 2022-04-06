#include <vkEasy/Device.h>
#include <vkEasy/Graph.h>
#include <vkEasy/resources/StagingBuffer.h>

using namespace VK_EASY_NAMESPACE;

StagingBuffer::StagingBuffer()
    : Buffer()
{
    setMemoryUsage(VMA_MEMORY_USAGE_CPU_ONLY);
}

void StagingBuffer::setData(const std::vector<uint8_t>& data)
{
    setUpdateData(true);
    m_data = data;
    setSize(m_data.size());
}

void StagingBuffer::getData(std::vector<uint8_t>& data, size_t offset)
{
    auto mapped = reinterpret_cast<uint8_t*>(m_vmaResource[getActualFrameIndex()]->mapMemory());
    memcpy(data.data(), mapped + offset, data.size());
    m_vmaResource[getActualFrameIndex()]->unmapMemory();
}

void StagingBuffer::update()
{
    Buffer::update();

    if (m_updateData[getActualFrameIndex()]) {
        void* mapped = m_vmaResource[getActualFrameIndex()]->mapMemory();
        memcpy(mapped, m_data.data(), m_data.size());
        m_vmaResource[getActualFrameIndex()]->unmapMemory();
        m_updateData[getActualFrameIndex()] = false;
    }
}
void StagingBuffer::setUpdateData(bool update)
{
    for (size_t i = 0; i < std::max(m_updateData.size(), static_cast<size_t>(1)); i++)
        m_updateData[i] = update;
}
