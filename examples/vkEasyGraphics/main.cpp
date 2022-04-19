#include <vkEasy/vkEasy.h>

int main()
{
    vk::easy::Context::get().setDebugOutput(true);
    auto& device = vk::easy::Context::get().createDevice();
    auto& graph = device.createGraph();
    auto& window = graph.getGLFWWindow(800, 600, "Graphics Test");
    auto& framebuffer = graph.createFramebuffer();
    graph.setNumberOfFramesInFlight(2);
    framebuffer.setWindow(window);

    auto& graphics = graph.createGraphicsNode();
    graphics.setFramebuffer(framebuffer);
    graphics.setColorAttachment(window.getAttachment(), 0);
    graphics.getVertexShaderStage().setShaderFile("shader.vert");
    graphics.getFragmentShaderStage().setShaderFile("shader.frag");
    graphics.setCullImmune(true);
    graphics.setNumberOfVertices(3);

    graph.enqueueNode(graphics);
    graph.compile();

    while (!window.shouldClose())
        graph.execute();

    return 0;
}
