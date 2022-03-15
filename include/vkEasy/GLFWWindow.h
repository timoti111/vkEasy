#pragma once

#include <vkEasy/WSI.h>
#include <vkEasy/global.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace VK_EASY_NAMESPACE {
class Device;
class GLFWWindow : public WSI {
    friend class Device;

public:
    GLFWWindow(GLFWWindow const&) = delete;
    void operator=(GLFWWindow const&) = delete;

    static std::vector<std::string> requiredInstanceExtensions();

    virtual vk::Extent2D resolution() override;

private:
    GLFWWindow(uint32_t width, uint32_t height, const std::string& title, Device* parent);
    GLFWwindow* m_handle;
};
} // namespace VK_EASY_NAMESPACE
