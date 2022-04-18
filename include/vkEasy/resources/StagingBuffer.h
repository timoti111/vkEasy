#pragma once
#include <vkEasy/global.h>
#include <vkEasy/resources/base/Buffer.h>

namespace VK_EASY_NAMESPACE {
class StagingBuffer : public Buffer {
    friend class Graph;

public:
    StagingBuffer(StagingBuffer const&) = delete;
    void operator=(StagingBuffer const&) = delete;

protected:
    StagingBuffer();
};
}
