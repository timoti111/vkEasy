#pragma once
#include <vkEasy/MemoryAllocator.h>
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

    bool isHostMemory();
    bool isBuffer();
    vk::DescriptorType getDescriptorType();

protected:
    Resource() = default;
    void setMemoryUsage(VmaMemoryUsage flag);
    virtual void update() = 0;
    virtual void create() = 0;
    virtual bool exists() = 0;

    VmaAllocationCreateInfo m_allocInfo;

    Graph* m_graph = nullptr;
    Device* m_device = nullptr;
    vk::DescriptorType m_descriptorType;
    bool m_isHostMemory = false;
    bool m_isBuffer = false;

private:
    void setGraph(Graph* graph);
};
}
