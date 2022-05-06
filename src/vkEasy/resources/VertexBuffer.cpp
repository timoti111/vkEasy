#include <vkEasy/resources/VertexBuffer.h>

using namespace VK_EASY_NAMESPACE;

VertexBuffer::VertexBuffer()
    : Buffer()
{
    setMemoryUsage(VMA_MEMORY_USAGE_GPU_ONLY);
    addBufferUsageFlag(vk::BufferUsageFlagBits::eVertexBuffer);
}
