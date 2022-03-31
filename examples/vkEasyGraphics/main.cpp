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
    auto& window = device.createGLFWWindow(800, 600, "Graphics Test");

    auto& graphics = graph.createGraphicsNode();

    auto& vertexStage = graphics.getVertexShaderStage();
    vertexStage.setShaderFile("shader.vert");
    auto& fragmentStage = graphics.getFragmentShaderStage();
    fragmentStage.setShaderFile("shader.frag");

    graph.enqueueNode(graphics);
    graph.compile();

    graph.execute();

    device.wait();

    std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}
