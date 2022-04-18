#include <vkEasy/resources/IndexBuffer.h>

using namespace VK_EASY_NAMESPACE;

IndexBuffer::IndexBuffer()
    : Buffer()
{
    m_descriptorType = vk::DescriptorType::eStorageBuffer;
    setMemoryUsage(VMA_MEMORY_USAGE_GPU_ONLY);
    addBufferUsageFlag(vk::BufferUsageFlagBits::eIndexBuffer);
}

void IndexBuffer::setIndices(const std::vector<uint16_t>& indices)
{
    m_indexType = vk::IndexType::eUint16;
    setIndicesImpl(indices);
}

void IndexBuffer::setIndices(const std::vector<uint32_t>& indices)
{
    m_indexType = vk::IndexType::eUint32;
    setIndicesImpl(indices);
}
