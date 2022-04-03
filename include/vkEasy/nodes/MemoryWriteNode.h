#pragma once
#include <vkEasy/nodes/base/Node.h>

namespace VK_EASY_NAMESPACE {
class StagingBuffer;
class BufferCopyNode;
class MemoryWriteNode : public Node {
    friend class Graph;

public:
    MemoryWriteNode(MemoryWriteNode const&) = delete;
    void operator=(MemoryWriteNode const&) = delete;

    void setDstResource(Resource& resource);
    void setData(const std::vector<uint8_t>& data, size_t offset = 0);

protected:
    MemoryWriteNode();
    void update();

private:
    size_t m_offset;
    Resource* m_resource;
    StagingBuffer* m_stagingBuffer = nullptr;
    BufferCopyNode* m_bufferCopyNode = nullptr;
    const std::vector<uint8_t>* m_data;
    bool m_updateData = false;
};
} // namespace VK_EASY_NAMESPACE