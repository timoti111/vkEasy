#pragma once
#include <vkEasy/Node.h>

namespace VK_EASY_NAMESPACE {
class ResourceWriteNode : public Node {
public:
    void writesTo(const vk::ArrayProxyNoTemporaries<Resource>& resources);
    void onUpdate(std::function<void(ResourceWriteNode&)> updateFunction);
    void setData(const std::vector<uint32_t>& data, size_t offset = 0);
};
} // namespace VK_EASY_NAMESPACE