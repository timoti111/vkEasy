#define GLM_FORCE_RADIANS
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <vkEasy/vkEasy.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
        glm::vec2 uv;
    };
    const std::vector<Vertex> vertices = { { { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
        { { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
        { { 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
        { { -0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } } };
    auto& vertexBuffer = graph.createVertexBuffer();
    vertexBuffer.setVertices(vertices);

    const std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0 };
    auto& indexBuffer = graph.createIndexBuffer();
    indexBuffer.setIndices(indices);

    auto& uniformBuffer = graph.createUniformBuffer(vk::easy::Resource::CPU_TO_GPU);

    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;
    if (!pixels)
        throw std::runtime_error("failed to load texture image!");
    auto& textureImage = graph.createTextureImage();
    textureImage.setDimensions(vk::Extent3D(texWidth, texHeight, 1));
    textureImage.setDimensionality(vk::ImageType::e2D);
    textureImage.setData(pixels, imageSize);

    auto& graphics = graph.createGraphicsNode();
    graphics.setFramebuffer(framebuffer);
    graphics.setColorAttachment(window.getAttachment(), 0);
    graphics.getVertexShaderStage().setShaderFile("shader.vert");
    graphics.getFragmentShaderStage().setShaderFile("shader.frag");
    graphics.setCullImmune(true);
    graphics.defineAttribute<glm::vec2>(0, offsetof(Vertex, pos), sizeof(Vertex), &vertexBuffer);
    graphics.defineAttribute<glm::vec3>(1, offsetof(Vertex, color), sizeof(Vertex), &vertexBuffer);
    graphics.defineAttribute<glm::vec2>(2, offsetof(Vertex, uv), sizeof(Vertex), &vertexBuffer);
    graphics.setIndexBuffer(&indexBuffer);
    graphics.createDescriptor({ &uniformBuffer }, 0, 0);
    graphics.createDescriptor({ &textureImage }, 1, 0);

    graph.enqueueNode(graphics);
    graph.compile();

    struct UniformBufferObject {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };
    std::vector<UniformBufferObject> ubo;
    ubo.resize(1);
    while (!window.shouldClose()) {
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        ubo[0].model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo[0].view
            = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        ubo[0].proj = glm::perspective(
            glm::radians(45.0f), window.resolution().width / (float)window.resolution().height, 0.1f, 10.0f);
        uniformBuffer.setData(ubo);

        graph.execute();
    }

    stbi_image_free(pixels);
    return 0;
}
