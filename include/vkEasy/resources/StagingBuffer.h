#pragma once
#include <vkEasy/global.h>
#include <vkEasy/resources/base/Buffer.h>

namespace VK_EASY_NAMESPACE {
class StagingBuffer : public Buffer {
    friend class Graph;

public:
    StagingBuffer(StagingBuffer const&) = delete;
    void operator=(StagingBuffer const&) = delete;

    void setData(const std::vector<uint32_t>& data);
    void getData(std::vector<uint32_t>& data, size_t offset = 0);

protected:
    StagingBuffer();

    virtual void update() override;

    std::vector<uint32_t> m_data;
    bool m_updateData = false;
};
}
