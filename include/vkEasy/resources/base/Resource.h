#pragma once
#include <vkEasy/global.h>

namespace VK_EASY_NAMESPACE {
class Device;
class Graph;
class Node;

class Resource {
    friend class Graph;
    friend class Node;

public:
    Resource(Resource const&) = delete;
    void operator=(Resource const&) = delete;
    virtual ~Resource() = default;

    void addMemoryPropertyFlag(vk::MemoryPropertyFlagBits flag);
    bool isHostMemory();
    bool isBuffer();
    vk::DescriptorType getDescriptorType();

protected:
    Resource() = default;
    virtual void update() = 0;
    virtual void create() = 0;
    bool exists();

    vk::MemoryPropertyFlags m_memoryPropertyFlags;
    std::unique_ptr<vk::raii::DeviceMemory> m_memory;
    Graph* m_graph = nullptr;
    Device* m_device = nullptr;
    vk::DescriptorType m_descriptorType;
    bool m_isHostMemory = false;
    bool m_isBuffer = false;

private:
    void setGraph(Graph* graph);
};
}
