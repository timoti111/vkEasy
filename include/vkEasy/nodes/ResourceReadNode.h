#pragma once
#include <vkEasy/nodes/base/ResourceOperationNode.h>

namespace VK_EASY_NAMESPACE {
class ResourceReadNode : public ResourceOperationNode {
    friend class Graph;

public:
    ResourceReadNode(ResourceReadNode const&) = delete;
    void operator=(ResourceReadNode const&) = delete;

    void onUpdate(std::function<void(ResourceReadNode&)> update);
    void setDataToRead(size_t offset = 0, size_t size = VK_WHOLE_SIZE);
    void onDataReady(std::function<void(const std::vector<uint32_t>&)> dataReady);

protected:
    ResourceReadNode();

private:
    using ResourceOperationNode::writesTo;
};
} // namespace VK_EASY_NAMESPACE