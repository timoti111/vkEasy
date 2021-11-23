#pragma once
#include <vkEasy/global.h>

namespace VK_EASY_NAMESPACE {
class Device;
class Graph;
class Node;

class Resource {
    friend class Graph;
    friend class Device;
    friend class Node;

public:
    virtual ~Resource() = default;
    void addMemoryPropertyFlag(vk::MemoryPropertyFlagBits flag);
    bool isHostMemory();
    bool isBuffer();
    vk::DescriptorType getDescriptorType();
    //    Resource(Resource&&) = default;
    //    Resource(const Resource&) = default;
    //    Resource& operator=(Resource&&) = default;
    //    Resource& operator=(const Resource&) = default;
    //    ~Resource() = default;();

protected:
    virtual void update() = 0;
    virtual void create() = 0;
    bool exists();

    vk::MemoryPropertyFlags m_memoryPropertyFlags;
    std::unique_ptr<vk::raii::DeviceMemory> m_memory;

    Graph* m_graph = nullptr;
    Device* m_device = nullptr;
    bool m_isHostMemory = false;
    bool m_isBuffer = false;
    vk::DescriptorType m_descriptorType;

private:
    void setGraph(Graph* graph);
};
}
