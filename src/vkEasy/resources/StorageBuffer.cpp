#include <vkEasy/resources/StorageBuffer.h>

using namespace VK_EASY_NAMESPACE;

StorageBuffer::StorageBuffer()
    : Buffer()
{
    m_descriptorType = vk::DescriptorType::eStorageBuffer;
    setMemoryUsage(VMA_MEMORY_USAGE_GPU_ONLY);
    addBufferUsageFlag(vk::BufferUsageFlagBits::eStorageBuffer);
}