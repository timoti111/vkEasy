#include <iostream>
#include <vkEasy/Context.h>
#include <vkEasy/GLFWWindow.h>

using namespace VK_EASY_NAMESPACE;

GLFWWindow::GLFWWindow(uint32_t width, uint32_t height, const std::string& title, Graph* parent)
    : WSI(width, height, title, parent)
{
    struct glfwContext {
        glfwContext()
        {
            glfwInit();
            glfwSetErrorCallback(
                [](int error, const char* msg) { std::cerr << "glfw: " << error << "; " << msg << std::endl; });

            std::vector<std::string> extensions;
            uint32_t count;
            auto extensionsPtr = glfwGetRequiredInstanceExtensions(&count);
            extensions.insert(extensions.end(), extensionsPtr, extensionsPtr + count);
            Context::get().addExtensions(extensions);
        }

        ~glfwContext()
        {
            glfwTerminate();
        }
    };
    static glfwContext glfwCtx = glfwContext();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_handle = glfwCreateWindow(m_extent.width, m_extent.height, m_name.c_str(), nullptr, nullptr);
}

vk::Extent2D GLFWWindow::resolution()
{
    int width, height;
    glfwGetFramebufferSize(m_handle, &width, &height);
    VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
    return actualExtent;
}

void GLFWWindow::createSurface()
{
    VkSurfaceKHR surface;
    auto& instance = vk::easy::Context::get().instance();
    VkResult err = glfwCreateWindowSurface(static_cast<VkInstance>(*instance), m_handle, nullptr, &surface);
    if (err != VK_SUCCESS)
        throw std::runtime_error("Failed to create window!");
    m_surface = std::make_unique<vk::raii::SurfaceKHR>(instance, surface);
}

bool GLFWWindow::shouldClose()
{
    return glfwWindowShouldClose(m_handle);
}

void GLFWWindow::pollEvents()
{
    glfwPollEvents();
}
