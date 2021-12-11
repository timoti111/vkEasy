#include <vkEasy/Device.h>
#include <vkEasy/resources/StagingBuffer.h>

using namespace VK_EASY_NAMESPACE;

StagingBuffer::StagingBuffer()
    : Buffer()
{
    setMemoryUsage(VMA_MEMORY_USAGE_CPU_ONLY);
}

void StagingBuffer::setData(const std::vector<uint8_t>& data)
{
    m_updateData = true;
    m_data = data;
    setSize(m_data.size());
}

void StagingBuffer::getData(std::vector<uint8_t>& data, size_t offset)
{
    auto mapped = reinterpret_cast<uint8_t*>(m_vmaResource->mapMemory());
    memcpy(data.data(), mapped + offset, data.size());
    m_vmaResource->unmapMemory();
}

void StagingBuffer::update()
{
    Buffer::update();

    if (m_updateData) {
        void* mapped = m_vmaResource->mapMemory();
        memcpy(mapped, m_data.data(), m_data.size());
        m_vmaResource->unmapMemory();
    }

    m_updateData = false;
}