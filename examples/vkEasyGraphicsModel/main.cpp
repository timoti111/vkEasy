#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>
#include <vkEasy/vkEasy.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

const std::string MODEL_PATH = "viking_room.obj";
const std::string TEXTURE_PATH = "viking_room.png";

struct Vertex {
    glm::vec3 pos;
    glm::vec2 texCoord;
    bool operator==(const Vertex& other) const
    {
        return pos == other.pos && texCoord == other.texCoord;
    }
};
namespace std {
template <> struct hash<Vertex> {
    size_t operator()(Vertex const& vertex) const
    {
        return hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
    }
};
}

int main()
{
    vk::easy::Context::get().setDebugOutput(true);
    auto& device = vk::easy::Context::get().createDevice();
    auto& graph = device.createGraph();
    auto& window = graph.getGLFWWindow(800, 600, "Graphics Test");
    auto& framebuffer = graph.createFramebuffer();
    graph.setNumberOfFramesInFlight(2);
    framebuffer.setWindow(window);
    auto depthStencilAttachment = framebuffer.getDepthStencilAttachment();

    std::vector<Vertex> vertices;
    std::vector<uint16_t> indices;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
        throw std::runtime_error(warn + err);
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices {};
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex {};
            vertex.pos = { attrib.vertices[3 * index.vertex_index + 0], attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2] };
            vertex.texCoord = { attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1] };
            vertices.push_back(vertex);
            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }
            indices.push_back(uniqueVertices[vertex]);
        }
    }

    auto& vertexBuffer = graph.createVertexBuffer();
    vertexBuffer.setVertices(vertices);

    auto& indexBuffer = graph.createIndexBuffer();
    indexBuffer.setIndices(indices);

    auto& uniformBuffer = graph.createUniformBuffer(vk::easy::Resource::CPU_TO_GPU);

    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;
    if (!pixels)
        throw std::runtime_error("failed to load texture image!");
    auto& textureImage = graph.createTextureImage();
    textureImage.setPersistence(true);
    textureImage.setDimensions(vk::Extent3D(texWidth, texHeight, 1));
    textureImage.setDimensionality(vk::ImageType::e2D);
    textureImage.setData(pixels, imageSize);

    auto& graphics = graph.createGraphicsNode();
    graphics.setFramebuffer(framebuffer);
    graphics.setColorAttachment(window.getAttachment(), 0);
    graphics.setDepthStencilAttachment(depthStencilAttachment);
    graphics.getVertexShaderStage().setShaderFile("shader.vert");
    graphics.getFragmentShaderStage().setShaderFile("shader.frag");
    graphics.setCullImmune(true);
    graphics.defineAttribute<glm::vec3>(0, offsetof(Vertex, pos), sizeof(Vertex), &vertexBuffer);
    graphics.defineAttribute<glm::vec2>(1, offsetof(Vertex, texCoord), sizeof(Vertex), &vertexBuffer);
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
