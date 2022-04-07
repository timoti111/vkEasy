#include <iostream>
#include <vkEasy/Device.h>
#include <vkEasy/Graph.h>
#include <vkEasy/nodes/base/Node.h>

using namespace VK_EASY_NAMESPACE;

Node::Node(const std::string& nodeName)
    : Errorable(nodeName)
    , Object()
{
}

void Node::uses(Resource* resource, Resource::Access access)
{
    if (resource->getDevice() != getDevice())
        return; // TODO Error
    if (access == Resource::Access::ReadOnly && !m_writes.contains(resource))
        m_reads.insert(resource);
    if (access == Resource::Access::ReadWrite) {
        m_writes.insert(resource);
        m_reads.erase(resource);
    }
}

void Node::addExecutionBarrier(vk::PipelineStageFlagBits src, vk::PipelineStageFlagBits dst)
{
    if (dst == vk::PipelineStageFlagBits::eNoneKHR || src == vk::PipelineStageFlagBits::eNoneKHR)
        return;

#ifndef NDEBUG
    std ::cout << "Adding execution barrier between { " << vk::to_string(src) << " } and { " << vk::to_string(dst)
               << " }" << std::endl;
#endif
    auto buffers = getCommandBuffers(1);
    if (buffers.empty())
        return;
    buffers[0]->pipelineBarrier(src, dst, {}, {}, {}, {});
}

void Node::addBufferBarrier(vk::PipelineStageFlags src, vk::PipelineStageFlags dst, vk::Buffer buffer,
    vk::AccessFlagBits srcMask, vk::AccessFlagBits dstMask)
{
#ifndef NDEBUG
    std ::cout << "Adding execution barrier between { " << vk::to_string(src) << " } and { " << vk::to_string(dst)
               << " }" << std::endl;
#endif
    vk::BufferMemoryBarrier bufferBarrier;
    bufferBarrier.setBuffer(buffer)
        .setSize(VK_WHOLE_SIZE)
        .setSrcAccessMask(srcMask)
        .setDstAccessMask(dstMask)
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
    auto buffers = getCommandBuffers(1);
    if (buffers.empty())
        return;
    buffers[0]->pipelineBarrier(src, dst, {}, {}, bufferBarrier, {});
}

void Node::addResourceEvent(std::function<void()> event, Resource* resource)
{
    auto& lastAccess = resource->getLastAccess();
    auto stage = lastAccess ? lastAccess->node->m_pipelineStage : vk::PipelineStageFlagBits::eNone;
    addEvent(event, stage);
}

void Node::addEvent(std::function<void()> event, vk::PipelineStageFlagBits afterStage)
{
    if (afterStage == vk::PipelineStageFlagBits::eNoneKHR)
        afterStage = vk::PipelineStageFlagBits::eAllCommands;
#ifndef NDEBUG
    std::cout << "Adding event after { " << vk::to_string(afterStage) << " }" << std::endl;
#endif
    auto vkEvent = getGraph()->createEvent(event);
    auto buffers = getCommandBuffers(1);
    if (buffers.empty())
        return;
    buffers[0]->setEvent(**vkEvent, afterStage);
}

void Node::execute()
{
    for (auto& read : m_reads) {
        if (!read->exists())
            read->update();
        auto& lastAccess = read->getLastAccess();
        if (!lastAccess)
            continue;
        if (lastAccess->access == Resource::Access::ReadWrite)
            addExecutionBarrier(lastAccess->node->m_pipelineStage, m_pipelineStage);
    }

    for (auto& write : m_writes) {
        if (!write->exists())
            write->update();
        auto& lastAccess = write->getLastAccess();
        if (!lastAccess)
            continue;
        addExecutionBarrier(lastAccess->node->m_pipelineStage, m_pipelineStage);
    }

#ifndef NDEBUG
    std::cout << "Executing { " << objectName() << " }" << std::endl;
#endif

    update();

    if (m_pipelineStage != vk::PipelineStageFlagBits::eNoneKHR) {
        for (auto& create : m_creates)
            create->getLastAccess() = Resource::AccessInfo { Resource::Access::ReadWrite, this };
        for (auto& read : m_reads)
            read->getLastAccess() = Resource::AccessInfo { Resource::Access::ReadOnly, this };
        for (auto& write : m_writes)
            write->getLastAccess() = Resource::AccessInfo { Resource::Access::ReadWrite, this };
    }
}

void Node::needsExtensions(const std::initializer_list<std::string>& extensions)
{
    for (auto& extension : extensions)
        m_neededExtensions.insert(extension);
}

void Node::setCullImmune(bool cullImmune)
{
    m_cullImmune = cullImmune;
}

std::vector<vk::raii::CommandBuffer*> Node::getCommandBuffers(size_t count)
{
    return getGraph()->getCommandBuffers(count);
}
