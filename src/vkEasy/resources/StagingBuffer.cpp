#include <vkEasy/Device.h>
#include <vkEasy/resources/StagingBuffer.h>

using namespace VK_EASY_NAMESPACE;

StagingBuffer::StagingBuffer()
    : Buffer()
{
    addMemoryPropertyFlag(vk::MemoryPropertyFlagBits::eHostVisible);
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
        auto mapped = m_memory->mapMemory(0, m_size);
        memcpy(mapped, m_data.data(), m_size);
        m_memory->unmapMemory();

        mapped = m_memory->mapMemory(0, VK_WHOLE_SIZE);
        vk::MappedMemoryRange mappedRange;
        mappedRange.setMemory(**m_memory).setOffset(0).setSize(VK_WHOLE_SIZE);
        m_device->getLogicalDevice()->flushMappedMemoryRanges(mappedRange);
        m_memory->unmapMemory();
    }

    m_updateData = false;
}

void StagingBuffer::getData(std::vector<uint32_t>& data, size_t offset)
{
    // Make device writes visible to the host
    auto mapped = m_memory->mapMemory(offset, VK_WHOLE_SIZE);
    vk::MappedMemoryRange mappedRange;
    mappedRange.setMemory(**m_memory).setOffset(offset).setSize(VK_WHOLE_SIZE);
    m_device->getLogicalDevice()->invalidateMappedMemoryRanges(mappedRange);

    // Copy to output
    memcpy(data.data(), mapped, data.size() * sizeof(uint32_t));
    m_memory->unmapMemory();

    // auto mapped = m_memory->mapMemory(offset, size);
    // vk::MappedMemoryRange mappedRange;
    // mappedRange.setMemory(**m_memory).setOffset(offset).setSize(size);
    // m_device->getLogicalDevice()->invalidateMappedMemoryRanges(mappedRange);
    // // Copy to output
    // memcpy(data.data(), mapped, size);
    // m_memory->unmapMemory();
    // return data;
}