#pragma once
#include <vkEasy/nodes/base/Node.h>
#include <vkEasy/resources/base/Buffer.h>

namespace VK_EASY_NAMESPACE {
class BufferCopyNode : public Node {
    friend class Graph;

public:
    BufferCopyNode(BufferCopyNode const&) = delete;
    void operator=(BufferCopyNode const&) = delete;

    void setSrcBuffer(Buffer* buffer, size_t size = VK_WHOLE_SIZE, size_t offset = 0);
    void setDstBuffer(Buffer* buffer, size_t offset = 0);

    void onUpdate(std::function<void(BufferCopyNode&)> update);

protected:
    BufferCopyNode();

private:
    std::function<void()> m_onUpdateFunction;
    vk::BufferCopy m_copyRegion;
    Buffer* m_src;
    Buffer* m_dst;
};
} // namespace VK_EASY_NAMESPACE