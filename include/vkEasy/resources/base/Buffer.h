#pragma once
#include <vkEasy/global.h>
#include <vkEasy/resources/base/Resource.h>

namespace VK_EASY_NAMESPACE {
class Buffer : public Resource {
public:
    Buffer(Buffer const&) = delete;
    void operator=(Buffer const&) = delete;
    virtual ~Buffer() = default;

    void addBufferUsageFlag(vk::BufferUsageFlagBits flag);
    vk::raii::Buffer* getVkBuffer();
    size_t getSize();
    void setSize(size_t size);

protected:
    Buffer();
    virtual void create();
    virtual void update();

    vk::BufferUsageFlags m_bufferUsageFlags;
    std::unique_ptr<vk::raii::Buffer> m_buffer;
    size_t m_size = 0;
    bool m_recreateBuffer = false;
};
}
