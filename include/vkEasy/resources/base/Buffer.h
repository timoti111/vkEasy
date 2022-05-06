#pragma once
#include <vkEasy/MemoryAllocator.h>
#include <vkEasy/global.h>
#include <vkEasy/resources/base/Resource.h>

namespace VK_EASY_NAMESPACE {
class MemoryCopyNode;
class Buffer : public Resource {
public:
    Buffer(Buffer const&) = delete;
    void operator=(Buffer const&) = delete;
    virtual ~Buffer() = default;

    void addBufferUsageFlag(vk::BufferUsageFlagBits flag);
    VkBuffer getVkBuffer();
    VkBuffer getVkBuffer(size_t index);
    size_t getSize();
    void setSize(size_t size);
    template <typename DataType> void setData(const std::vector<DataType>& data)
    {
        setData(reinterpret_cast<const uint8_t*>(data.data()), data.size() * sizeof(DataType));
    }
    void setData(const uint8_t* data, size_t size, size_t offset = 0);

protected:
    Buffer();
    virtual void create();
    void insertBufferBarrier(
        vk::PipelineStageFlagBits src, vk::PipelineStageFlagBits dst, const vk::BufferMemoryBarrier& barrier);
    virtual void transferFromStagingBuffer(StagingBuffer* stagingBuffer, size_t offset);
    virtual void transferToStagingBuffer(StagingBuffer* stagingBuffer, size_t offset);
    virtual void solveSynchronization(vk::PipelineStageFlagBits stage, Access access);

    vk::BufferUsageFlags m_bufferUsageFlags;
    vk::BufferCreateInfo m_bufferCreateInfo;
    std::map<size_t, VkBuffer> m_buffers;

    MemoryCopyNode* m_bufferCopyNode = nullptr;
};
}
