#pragma once
#include <map>
#include <optional>
#include <set>
#include <vkEasy/MemoryAllocator.h>
#include <vkEasy/Utils.h>
#include <vkEasy/global.h>

namespace VK_EASY_NAMESPACE {
class Graph;
class Node;
class StagingBuffer;

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
    void setPersistence(bool persistent);
    bool isPersistent();

    void setDataToRead(size_t size = VK_WHOLE_SIZE, size_t offset = 0);
    template <typename DataType> std::vector<DataType> getData()
    {
        std::vector<DataType> ret;
        auto data = getData();
        auto newDataPtr = reinterpret_cast<const DataType*>(data.data());
        ret.insert(ret.end(), newDataPtr, newDataPtr + data.size() / sizeof(DataType));
        return ret;
    }
    const std::vector<uint8_t>& getData();

    typedef enum OptimizationFlags {
        NO_OPTIMIZATION,
        CPU_TO_GPU = VMA_MEMORY_USAGE_CPU_TO_GPU,
        GPU_TO_CPU = VMA_MEMORY_USAGE_GPU_TO_CPU
    } OptimizationFlags;

protected:
    Resource() = default;

    struct AccessInfo {
        std::optional<vk::PipelineStageFlagBits> lastWrite;
        std::optional<vk::PipelineStageFlagBits> lastRead;
        std::optional<vk::AccessFlagBits> lastAccess;
    };

    virtual void create() = 0;
    virtual void transferFromStagingBuffer(StagingBuffer* stagingBuffer, size_t offset) = 0;
    virtual void transferToStagingBuffer(StagingBuffer* stagingBuffer, size_t offset) = 0;
    virtual void solveSynchronization(vk::PipelineStageFlagBits stage, Access access) = 0;
    MemoryAllocator::Resource& getMemory();
    void setMemoryUsage(VmaMemoryUsage flag);
    void destroy();
    virtual void update();
    virtual bool exists();
    size_t getActualFrameIndex();
    AccessInfo& getLastAccessInfo();
    virtual void setOptimization(OptimizationFlags optimization);
    void setWriteData(const uint8_t* data, size_t size, size_t offset);

    VmaAllocationCreateInfo m_allocInfo;

    vk::DescriptorType m_descriptorType;
    bool m_isBuffer = false;

    // Graph compilation members
    Node* m_creator = nullptr;
    std::vector<Node*> m_readers;
    std::vector<Node*> m_writers;
    size_t m_referenceCount;
    bool m_isPersistent = false;
    std::set<uint32_t> m_queueIndices;
    std::vector<uint32_t> m_queueIndicesVector;

    std::map<size_t, AccessInfo> m_lastAccess;
    std::map<size_t, std::unique_ptr<MemoryAllocator::Resource>> m_vmaResource;

private:
    size_t m_writeOffset;
    std::vector<uint8_t> m_writeData;
    StagingBuffer* m_writeStagingBuffer = nullptr;
    size_t m_readSize = 0;
    size_t m_readOffset;
    std::vector<uint8_t> m_readData;
    StagingBuffer* m_readStagingBuffer = nullptr;
    std::function<void(const std::vector<uint8_t>&)> m_onDataReady;
};
}
