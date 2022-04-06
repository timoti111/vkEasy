#pragma once
#include <vkEasy/global.h>
#include <vkEasy/resources/base/Buffer.h>

namespace VK_EASY_NAMESPACE {
class StagingBuffer : public Buffer {
    friend class Graph;
    friend class MemoryWriteNode;
    friend class MemoryReadNode;

public:
    StagingBuffer(StagingBuffer const&) = delete;
    void operator=(StagingBuffer const&) = delete;

protected:
    StagingBuffer();

    void setData(const std::vector<uint8_t>& data);
    void getData(std::vector<uint8_t>& data, size_t offset = 0);

    void update() override;
    void setUpdateData(bool update);

    std::vector<uint8_t> m_data;
    std::map<size_t, bool> m_updateData;
};
}
