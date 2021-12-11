#pragma once
#include <vkEasy/global.h>
#include <vkEasy/resources/base/Buffer.h>

namespace VK_EASY_NAMESPACE {
class UniformBuffer : public Buffer {
    friend class Graph;

public:
    UniformBuffer(UniformBuffer const&) = delete;
    void operator=(UniformBuffer const&) = delete;

protected:
    UniformBuffer();
};
}
