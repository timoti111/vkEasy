#include <vkEasy/resources/StorageBuffer.h>

using namespace VK_EASY_NAMESPACE;

StorageBuffer::StorageBuffer()
    : Buffer()
{
    m_descriptorType = vk::DescriptorType::eStorageBuffer;
    addMemoryPropertyFlag(vk::MemoryPropertyFlagBits::eDeviceLocal);
    addBufferUsageFlag(vk::BufferUsageFlagBits::eStorageBuffer);
}