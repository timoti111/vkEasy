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

    template <typename Vertex> void setVertices(const std::vector<Vertex>& vertices)
    {
        m_numberOfVertices = vertices.size();
        setData(reinterpret_cast<const uint8_t*>(vertices.data()), vertices.size() * sizeof(Vertex));
    }

protected:
    VertexBuffer();
    size_t m_numberOfVertices;
};
}
