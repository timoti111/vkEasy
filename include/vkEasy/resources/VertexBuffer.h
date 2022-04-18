#pragma once
#include <glm.hpp>
#include <vkEasy/global.h>
#include <vkEasy/resources/base/Buffer.h>

namespace VK_EASY_NAMESPACE {
class VertexBuffer : public Buffer {
    friend class Graph;
    friend class GraphicsNode;

public:
    VertexBuffer(VertexBuffer const&) = delete;
    void operator=(VertexBuffer const&) = delete;

    template <typename Attribute> void defineAttribute(uint32_t index);
    void defineAttribute(uint32_t index, vk::Format format, size_t size, uint32_t offset = ~0);
    void setPrimitiveTopology(vk::PrimitiveTopology topology);
    template <typename Vertex> void setVertices(const std::vector<Vertex>& vertices)
    {
        m_numberOfVertices = vertices.size();
        setData(reinterpret_cast<const uint8_t*>(vertices.data()), vertices.size() * sizeof(Vertex));
    }

protected:
    VertexBuffer();

    std::vector<vk::VertexInputAttributeDescription>& getAttributes();
    uint32_t getStride();
    struct Attribute {
        size_t size;
        vk::VertexInputAttributeDescription vkAttribute;
    };
    std::map<uint32_t, Attribute> m_attributes;
    std::vector<vk::VertexInputAttributeDescription> m_attributesCompiled;
    vk::PrimitiveTopology m_primitiveTopology;
    size_t m_numberOfAttributes = 0;
    size_t m_numberOfVertices;
};
}
