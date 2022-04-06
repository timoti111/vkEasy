#pragma once

#include <vkEasy/WSI.h>
#include <vkEasy/global.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace VK_EASY_NAMESPACE {
class Graph;
class GLFWWindow : public WSI {
    friend class Graph;

public:
    GLFWWindow(GLFWWindow const&) = delete;
    void operator=(GLFWWindow const&) = delete;

    virtual bool shouldClose() override;
    virtual void pollEvents() override;

protected:
    virtual vk::Extent2D osWindowResolution() override;
    virtual void createSurface() override;

private:
    GLFWWindow(uint32_t width, uint32_t height, const std::string& title, Graph* parent);
    GLFWwindow* m_handle;
};
} // namespace VK_EASY_NAMESPACE
