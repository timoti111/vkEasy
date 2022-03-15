#pragma once
#include <vkEasy/MemoryAllocator.h>
#include <vkEasy/global.h>
#include <vkEasy/resources/base/Resource.h>

namespace VK_EASY_NAMESPACE {
class Buffer : public Resource {
public:
    Buffer(Buffer const&) = delete;
    void operator=(Buffer const&) = delete;
    virtual ~Buffer() = default;

    void addBufferUsageFlag(vk::BufferUsageFlagBits flag);
    VkBuffer getVkBuffer();
    size_t getSize();
    void setSize(size_t size);

protected:
    Buffer();
    virtual void create();

    vk::BufferUsageFlags m_bufferUsageFlags;
    vk::BufferCreateInfo m_bufferCreateInfo;
};
}
