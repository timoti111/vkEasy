#pragma once
#include <map>
#include <optional>
#include <vkEasy/MemoryAllocator.h>
#include <vkEasy/Utils.h>
#include <vkEasy/global.h>

namespace VK_EASY_NAMESPACE {
class Graph;
class Node;

class Resource : public Object {
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

    struct AccessInfo {
        Access access;
        Node* node;
    };

    void setMemoryUsage(VmaMemoryUsage flag);
    virtual void create() = 0;
    void destroy();
    virtual void update();
    virtual bool exists();
    size_t getActualFrameIndex();
    void setRecreateResource(bool recreate);
    std::optional<AccessInfo>& getLastAccess();
    virtual void setOptimization(OptimizationFlags optimization);

    VmaAllocationCreateInfo m_allocInfo;

    vk::DescriptorType m_descriptorType;
    bool m_isBuffer = false;

    // Graph compilation members
    Node* m_creator = nullptr;
    std::vector<Node*> m_readers;
    std::vector<Node*> m_writers;
    size_t m_referenceCount;
    bool m_isPersistent = false;

    std::map<size_t, std::optional<AccessInfo>> m_lastAccess;
    std::map<size_t, bool> m_recreateResource;
    std::map<size_t, std::unique_ptr<MemoryAllocator::Resource>> m_vmaResource;
};
}
