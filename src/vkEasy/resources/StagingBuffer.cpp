#include <vkEasy/Device.h>
#include <vkEasy/resources/StagingBuffer.h>

using namespace VK_EASY_NAMESPACE;

StagingBuffer::StagingBuffer()
    : Buffer()
{
    setMemoryUsage(VMA_MEMORY_USAGE_CPU_ONLY);
}

void StagingBuffer::setData(const std::vector<uint32_t>& data)
{
    m_updateData = true;
    m_data = data;
    setSize(m_data.size() * sizeof(uint32_t));
}

void StagingBuffer::update()
{
    Buffer::update();

    if (m_updateData) {
        void* mapped = m_buffer->mapMemory();
        memcpy(mapped, m_data.data(), m_size);
        m_buffer->unmapMemory();
    }

    m_updateData = false;
}

void StagingBuffer::getData(std::vector<uint32_t>& data, size_t offset)
{
    void* mapped = m_buffer->mapMemory();
    memcpy(data.data(), mapped, data.size() * sizeof(uint32_t));
    m_buffer->unmapMemory();
}