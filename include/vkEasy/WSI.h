#pragma once
#include <vkEasy/Device.h>
#include <vkEasy/Error.h>
#include <vkEasy/global.h>

namespace VK_EASY_NAMESPACE {
class WSI : public Errorable {
    friend class Device;

public:
    WSI(WSI const&) = delete;
    void operator=(WSI const&) = delete;

protected:
    WSI();
    virtual VkSurfaceKHR getSurface() = 0;
};
} // namespace VK_EASY_NAMESPACE
