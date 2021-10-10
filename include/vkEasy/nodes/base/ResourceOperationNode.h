#pragma once
#include <vkEasy/nodes/base/Node.h>

namespace VK_EASY_NAMESPACE {
class ResourceOperationNode : public Node {
    friend class Graph;

public:
    ResourceOperationNode(ResourceOperationNode const&) = delete;
    void operator=(ResourceOperationNode const&) = delete;

    void readsFrom(Resource* resources);
    void writesTo(Resource* resources);

protected:
    ResourceOperationNode(const std::string& nodeName);
    using Node::readsFrom;
    using Node::writesTo;
};
} // namespace VK_EASY_NAMESPACE