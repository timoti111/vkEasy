#pragma once
#include <vkEasy/nodes/base/Node.h>
#include <vkEasy/resources/base/Resource.h>

namespace VK_EASY_NAMESPACE {
class BufferCopyNode : public Node {
    friend class Graph;

public:
    BufferCopyNode(BufferCopyNode const&) = delete;
    void operator=(BufferCopyNode const&) = delete;

    void setSrcResource(Resource& resource, size_t size = VK_WHOLE_SIZE, size_t offset = 0);
    void setDstResource(Resource& resource, size_t offset = 0);

protected:
    BufferCopyNode();
    void update();

private:
    vk::BufferCopy m_copyRegion;
    Resource* m_src;
    Resource* m_dst;
};
} // namespace VK_EASY_NAMESPACE