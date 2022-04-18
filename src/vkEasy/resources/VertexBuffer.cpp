#include <vkEasy/resources/VertexBuffer.h>

using namespace VK_EASY_NAMESPACE;

VertexBuffer::VertexBuffer()
    : Buffer()
{
    setPrimitiveTopology(vk::PrimitiveTopology::eTriangleList);
    setMemoryUsage(VMA_MEMORY_USAGE_GPU_ONLY);
    addBufferUsageFlag(vk::BufferUsageFlagBits::eVertexBuffer);
}

void VertexBuffer::defineAttribute(uint32_t index, vk::Format format, size_t size, uint32_t offset)
{
    m_attributes[index].size = size;
    m_attributes[index].vkAttribute.setFormat(format);
    m_attributes[index].vkAttribute.setOffset(offset);
}

template <> void VertexBuffer::defineAttribute<int>(uint32_t index)
{
    defineAttribute(index, vk::Format::eR32Sint, sizeof(int));
}

template <> void VertexBuffer::defineAttribute<unsigned>(uint32_t index)
{
    defineAttribute(index, vk::Format::eR32Uint, sizeof(unsigned));
}

template <> void VertexBuffer::defineAttribute<float>(uint32_t index)
{
    defineAttribute(index, vk::Format::eR32Sfloat, sizeof(float));
}

template <> void VertexBuffer::defineAttribute<double>(uint32_t index)
{
    defineAttribute(index, vk::Format::eR64Sfloat, sizeof(double));
}

template <> void VertexBuffer::defineAttribute<glm::vec1>(uint32_t index)
{
    defineAttribute(index, vk::Format::eR32Sfloat, sizeof(glm::vec1));
}

template <> void VertexBuffer::defineAttribute<glm::vec2>(uint32_t index)
{
    defineAttribute(index, vk::Format::eR32G32Sfloat, sizeof(glm::vec2));
}

template <> void VertexBuffer::defineAttribute<glm::vec3>(uint32_t index)
{
    defineAttribute(index, vk::Format::eR32G32B32Sfloat, sizeof(glm::vec3));
}

template <> void VertexBuffer::defineAttribute<glm::vec4>(uint32_t index)
{
    defineAttribute(index, vk::Format::eR32G32B32A32Sfloat, sizeof(glm::vec4));
}

template <> void VertexBuffer::defineAttribute<glm::uvec1>(uint32_t index)
{
    defineAttribute(index, vk::Format::eR32Uint, sizeof(glm::uvec1));
}

template <> void VertexBuffer::defineAttribute<glm::uvec2>(uint32_t index)
{
    defineAttribute(index, vk::Format::eR32G32Uint, sizeof(glm::uvec2));
}

template <> void VertexBuffer::defineAttribute<glm::uvec3>(uint32_t index)
{
    defineAttribute(index, vk::Format::eR32G32B32Uint, sizeof(glm::uvec3));
}

template <> void VertexBuffer::defineAttribute<glm::uvec4>(uint32_t index)
{
    defineAttribute(index, vk::Format::eR32G32B32A32Uint, sizeof(glm::uvec4));
}

template <> void VertexBuffer::defineAttribute<glm::ivec1>(uint32_t index)
{
    defineAttribute(index, vk::Format::eR32Sint, sizeof(glm::ivec1));
}

template <> void VertexBuffer::defineAttribute<glm::ivec2>(uint32_t index)
{
    defineAttribute(index, vk::Format::eR32G32Sint, sizeof(glm::ivec2));
}

template <> void VertexBuffer::defineAttribute<glm::ivec3>(uint32_t index)
{
    defineAttribute(index, vk::Format::eR32G32B32Sint, sizeof(glm::ivec3));
}

template <> void VertexBuffer::defineAttribute<glm::ivec4>(uint32_t index)
{
    defineAttribute(index, vk::Format::eR32G32B32A32Sint, sizeof(glm::ivec4));
}

void VertexBuffer::setPrimitiveTopology(vk::PrimitiveTopology topology)
{
    m_primitiveTopology = topology;
}

std::vector<vk::VertexInputAttributeDescription>& VertexBuffer::getAttributes()
{
    m_attributesCompiled.clear();
    size_t offset = 0;
    size_t location = 0;
    for (auto& [key, elem] : m_attributes) {
        // if (elem.vkAttribute.offset == ~static_cast<uint32_t>(0))
        elem.vkAttribute.setOffset(offset);
        elem.vkAttribute.setLocation(location);
        elem.vkAttribute.setBinding(0);
        m_attributesCompiled.push_back(elem.vkAttribute);
        offset += elem.size;
        offset += offset % 8;
        location = offset / 8;
    }
    return m_attributesCompiled;
}

uint32_t VertexBuffer::getStride()
{
    getAttributes();
    if (!m_attributesCompiled.empty())
        return m_attributesCompiled.back().offset + (--m_attributes.end())->second.size;
    return 0;
}
