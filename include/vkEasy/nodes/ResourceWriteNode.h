#pragma once
#include <vkEasy/nodes/base/ResourceOperationNode.h>

namespace VK_EASY_NAMESPACE {
class ResourceWriteNode : public ResourceOperationNode {
    friend class Graph;

public:
    ResourceWriteNode(ResourceWriteNode const&) = delete;
    void operator=(ResourceWriteNode const&) = delete;

    void onUpdate(std::function<void(ResourceWriteNode&)> updateFunction);
    void setData(const std::vector<uint32_t>& data, size_t offset = 0);

protected:
    ResourceWriteNode();

private:
    using ResourceOperationNode::readsFrom;
};
} // namespace VK_EASY_NAMESPACE