#pragma once
#include <vkEasy/nodes/base/Node.h>

namespace VK_EASY_NAMESPACE {
class PresentNode : public Node {
    friend class Graph;

public:
    PresentNode(PresentNode const&) = delete;
    void operator=(PresentNode const&) = delete;

protected:
    PresentNode();
    void update(Device* device);

private:
    const std::vector<uint8_t>& getData();
};
} // namespace VK_EASY_NAMESPACE