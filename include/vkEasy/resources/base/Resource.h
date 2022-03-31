#pragma once
#include <optional>
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

    enum class Access { ReadOnly = 1, ReadWrite = 2 };

    bool isBuffer();
    vk::DescriptorType getDescriptorType();
    MemoryAllocator::Resource& getMemory();
    void setPersistence(bool persistent);

    typedef enum OptimizationFlags {
        NO_OPTIMIZATION,
        CPU_TO_GPU = VMA_MEMORY_USAGE_CPU_TO_GPU,
        GPU_TO_CPU = VMA_MEMORY_USAGE_GPU_TO_CPU
    } OptimizationFlags;

protected:
    Resource() = default;

    void setMemoryUsage(VmaMemoryUsage flag);
    virtual void create() = 0;
    void destroy();
    virtual void update();
    virtual bool exists();

    virtual void setOptimization(OptimizationFlags optimization);

    VmaAllocationCreateInfo m_allocInfo;

    struct AccessInfo {
        Access access;
        Node* node;
    };

    std::optional<AccessInfo> m_lastAccess;

    Graph* m_graph = nullptr;
    Device* m_device = nullptr;
    vk::DescriptorType m_descriptorType;
    bool m_isBuffer = false;
    bool m_recreateResource = false;

    // Graph compilation members
    Node* m_creator = nullptr;
    std::vector<Node*> m_readers;
    std::vector<Node*> m_writers;
    size_t m_referenceCount;
    bool m_isPersistent = false;

    std::unique_ptr<MemoryAllocator::Resource> m_vmaResource;

private:
    void setGraph(Graph* graph);
};
}
