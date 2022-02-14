#pragma once
#include <vkEasy/WSI.h>
#include <vkEasy/global.h>

namespace VK_EASY_NAMESPACE {
class GLFWWindow : public WSI {
    friend class Device;

public:
    GLFWWindow(GLFWWindow const&) = delete;
    void operator=(GLFWWindow const&) = delete;

protected:
    VkSurfaceKHR getSurface();

private:
    GLFWWindow();
};
} // namespace VK_EASY_NAMESPACE
