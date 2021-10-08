#pragma once
#include <vkEasy/Node.h>

namespace VK_EASY_NAMESPACE {
class ResourceReadNode : public Node {
public:
    void readsFrom(const vk::ArrayProxyNoTemporaries<Resource>& resources);
    void onUpdate(std::function<void(ResourceReadNode&)> update);
    void setDataToRead(size_t offset = 0, size_t size = VK_WHOLE_SIZE);
    void onDataReady(std::function<void(const std::vector<uint32_t>&)> dataReady);
};
} // namespace VK_EASY_NAMESPACE