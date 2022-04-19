#include <vkEasy/Device.h>
#include <vkEasy/Graph.h>
#include <vkEasy/resources/base/Resource.h>

using namespace VK_EASY_NAMESPACE;

bool Resource::isBuffer()
{
    return m_isBuffer;
}

void Resource::setMemoryUsage(VmaMemoryUsage usage)
{
    if (usage == VMA_MEMORY_USAGE_GPU_ONLY)
        m_allocInfo.flags &= ~VMA_ALLOCATION_CREATE_MAPPED_BIT;
    else
        m_allocInfo.flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;

    m_allocInfo.usage = usage;
}

vk::DescriptorType Resource::getDescriptorType()
{
    return m_descriptorType;
}

MemoryAllocator::Resource& Resource::getMemory()
{
    return *m_vmaResource[getActualFrameIndex()].get();
}

void Resource::setOptimization(OptimizationFlags optimization)
{
    if (m_allocInfo.usage == VMA_MEMORY_USAGE_CPU_ONLY || optimization == NO_OPTIMIZATION)
        return;
    setMemoryUsage(static_cast<VmaMemoryUsage>(optimization));
}

bool Resource::exists()
{
    return static_cast<bool>(m_vmaResource[getActualFrameIndex()]);
}

void Resource::update()
{
    if (!exists()) {
        m_lastAccess[getActualFrameIndex()] = AccessInfo();
        m_queueIndicesVector.clear();
        m_queueIndicesVector.insert(m_queueIndicesVector.end(), m_queueIndices.begin(), m_queueIndices.end());
        create();
    }

    if (!m_writeData.empty()) {
        if (getMemory().isMappable()) {
            char* mapped = reinterpret_cast<char*>(m_vmaResource[getActualFrameIndex()]->mapMemory());
            memcpy(mapped + m_writeOffset, m_writeData.data(), m_writeData.size());
            m_vmaResource[getActualFrameIndex()]->unmapMemory();
            getLastAccessInfo().lastWrite = vk::PipelineStageFlagBits::eHost;
            getLastAccessInfo().lastAccess = vk::AccessFlagBits::eHostWrite;
        } else {
            if (!m_writeStagingBuffer)
                m_writeStagingBuffer = &getGraph()->createStagingBuffer();
            m_writeStagingBuffer->setData(m_writeData);
            m_writeStagingBuffer->update();
            transferFromStagingBuffer(m_writeStagingBuffer, m_writeOffset);
        }
    }

    if (m_readSize != 0) {
        m_readSize = m_readSize == VK_WHOLE_SIZE ? getMemory().getSize() : m_readSize;
        if (!getMemory().isMappable()) {
            if (!m_readStagingBuffer)
                m_readStagingBuffer = &getGraph()->createStagingBuffer();
            m_readStagingBuffer->setDataToRead(m_readSize);
            m_readStagingBuffer->setSize(m_readSize);
            m_readStagingBuffer->update();
            getGraph()->pushCommand([this] { transferToStagingBuffer(m_readStagingBuffer, m_readOffset); });
        }
    }
}

void Resource::destroy()
{
    if (m_writeStagingBuffer)
        m_writeStagingBuffer->destroy();
    if (m_readStagingBuffer)
        m_readStagingBuffer->destroy();
    if (m_isPersistent)
        return;
    m_vmaResource[getActualFrameIndex()].reset();
}

void Resource::setPersistence(bool persistent)
{
    m_isPersistent = persistent;
}

size_t Resource::getActualFrameIndex()
{
    return m_isPersistent ? 0 : getGraph()->getImageIndex();
}

Resource::AccessInfo& Resource::getLastAccessInfo()
{
    return m_lastAccess[getActualFrameIndex()];
}

bool Resource::isPersistent()
{
    return m_isPersistent;
}

void Resource::setWriteData(const uint8_t* data, size_t size, size_t offset)
{
    m_writeOffset = offset;
    m_writeData.clear();
    m_writeData.insert(m_writeData.end(), data, data + size);
}

void Resource::setDataToRead(size_t size, size_t offset)
{
    m_readSize = size;
    m_readOffset = offset;
}

const std::vector<uint8_t>& Resource::getData()
{
    m_readData.clear();
    getDevice()->wait();
    if (getMemory().isMappable()) {
        char* mapped = reinterpret_cast<char*>(m_vmaResource[getActualFrameIndex()]->mapMemory());
        m_readData.insert(m_readData.end(), mapped + m_readOffset, mapped + m_readOffset + m_readSize);
        m_vmaResource[getActualFrameIndex()]->unmapMemory();
    } else {
        if (m_readStagingBuffer)
            m_readData = m_readStagingBuffer->getData();
    }
    return m_readData;
}
