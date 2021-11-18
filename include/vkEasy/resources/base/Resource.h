#pragma once
#include <vkEasy/global.h>

namespace VK_EASY_NAMESPACE {
class Resource {
public:
    virtual ~Resource() = default;
    void create(vk::easy::Device* device);
    void destroy(vk::easy::Device* device);
    //    Resource(Resource&&) = default;
    //    Resource(const Resource&) = default;
    //    Resource& operator=(Resource&&) = default;
    //    Resource& operator=(const Resource&) = default;
    //    ~Resource() = default;

private:
};
}
