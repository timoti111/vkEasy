#include <chrono>
#include <iostream>
#include <thread>
#include <vkEasy/Context.h>
#include <vkEasy/GLFWWindow.h>
#include <vkEasy/Graph.h>
#include <vkEasy/nodes/GraphicsNode.h>

int main()
{
    auto& device = vk::easy::Context::get().createDevice();
    auto& graph = device.createGraph();
    auto& window = graph.getGLFWWindow(800, 600, "Graphics Test");
    auto& framebuffer = graph.createFramebuffer();
    framebuffer.setWindow(window);

    auto& graphics = graph.createGraphicsNode();
    graphics.setFramebuffer(framebuffer);
    graphics.setColorAttachment(window.getAttachment(), 0);
    graphics.getVertexShaderStage().setShaderFile("shader.vert");
    graphics.getFragmentShaderStage().setShaderFile("shader.frag");
    graphics.setCullImmune(true);

    graph.enqueueNode(graphics);
    graph.compile();

    while (!window.shouldClose())
        graph.execute();

    return 0;
}
