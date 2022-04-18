#include <vkEasy/resources/StagingBuffer.h>

using namespace VK_EASY_NAMESPACE;

StagingBuffer::StagingBuffer()
    : Buffer()
{
    setMemoryUsage(VMA_MEMORY_USAGE_CPU_ONLY);
}