#pragma once
#include <vkEasy/global.h>
#include <vkEasy/resources/base/Buffer.h>

namespace VK_EASY_NAMESPACE {
class IndexBuffer : public Buffer {
    friend class Graph;
    friend class GraphicsNode;

public:
    IndexBuffer(IndexBuffer const&) = delete;
    void operator=(IndexBuffer const&) = delete;

    void setIndices(const std::vector<uint16_t>& indices);
    void setIndices(const std::vector<uint32_t>& indices);

protected:
    IndexBuffer();

    template <typename IndexType> void setIndicesImpl(const std::vector<IndexType>& indices)
    {
        m_numberOfIndices = indices.size();
        setData(reinterpret_cast<const uint8_t*>(indices.data()), indices.size() * sizeof(IndexType));
    }
    vk::IndexType m_indexType = vk::IndexType::eUint32;
    size_t m_numberOfIndices;
};
}
