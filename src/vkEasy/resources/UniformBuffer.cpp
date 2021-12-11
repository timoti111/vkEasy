#include <vkEasy/resources/UniformBuffer.h>

using namespace VK_EASY_NAMESPACE;

UniformBuffer::UniformBuffer()
    : Buffer()
{
    m_descriptorType = vk::DescriptorType::eUniformBuffer;
    setMemoryUsage(VMA_MEMORY_USAGE_GPU_ONLY);
    addBufferUsageFlag(vk::BufferUsageFlagBits::eUniformBuffer);
}