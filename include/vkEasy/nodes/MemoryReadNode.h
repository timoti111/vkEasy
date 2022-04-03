#pragma once
#include <vkEasy/nodes/base/Node.h>

namespace VK_EASY_NAMESPACE {
class StagingBuffer;
class BufferCopyNode;
class MemoryReadNode : public Node {
    friend class Graph;

public:
    MemoryReadNode(MemoryReadNode const&) = delete;
    void operator=(MemoryReadNode const&) = delete;

    void setSrcResource(Resource& resource);
    void setDataToRead(size_t offset = 0, size_t size = VK_WHOLE_SIZE);
    void onDataReady(std::function<void(const std::vector<uint8_t>&)> dataReady);

protected:
    MemoryReadNode();
    void update();

private:
    const std::vector<uint8_t>& getData();

    size_t m_offset;
    size_t m_size;
    Resource* m_resource;
    StagingBuffer* m_stagingBuffer = nullptr;
    BufferCopyNode* m_bufferCopyNode = nullptr;
    std::function<void(const std::vector<uint8_t>&)> m_onDataReady;
    std::vector<uint8_t> m_data;
};
} // namespace VK_EASY_NAMESPACE