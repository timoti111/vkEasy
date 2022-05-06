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

    struct Vertex {
        glm::vec2 pos;
        glm::vec3 color;
    };
    const std::vector<Vertex> vertices
        = { { { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } }, { { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
              { { 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } }, { { -0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f } } };
    auto& vertexBuffer = graph.createVertexBuffer();
    vertexBuffer.setVertices(vertices);

    const std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0 };
    auto& indexBuffer = graph.createIndexBuffer();
    indexBuffer.setIndices(indices);

    auto& graphics = graph.createGraphicsNode();
    graphics.setFramebuffer(framebuffer);
    graphics.setColorAttachment(window.getAttachment(), 0);
    graphics.getVertexShaderStage().setShaderFile("shader.vert");
    graphics.getFragmentShaderStage().setShaderFile("shader.frag");
    graphics.setCullImmune(true);
    graphics.defineAttribute<glm::vec2>(0, offsetof(Vertex, pos), sizeof(Vertex), &vertexBuffer);
    graphics.defineAttribute<glm::vec3>(1, offsetof(Vertex, color), sizeof(Vertex), &vertexBuffer);
    graphics.setIndexBuffer(&indexBuffer);

    graph.enqueueNode(graphics);
    graph.compile();

    while (!window.shouldClose())
        graph.execute();

    return 0;
}
