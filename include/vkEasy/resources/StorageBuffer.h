#pragma once
#include <vkEasy/global.h>
#include <vkEasy/resources/base/Buffer.h>

namespace VK_EASY_NAMESPACE {
class StorageBuffer : public Buffer {
    friend class Graph;

public:
    StorageBuffer(StorageBuffer const&) = delete;
    void operator=(StorageBuffer const&) = delete;

protected:
    StorageBuffer();
};
}
