#include <iostream>
#include <vkEasy/Context.h>
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

void Node::execute()
{
    for (auto& create : m_creates) {
        if (!create->exists())
            create->update();
        create->solveSynchronization(m_pipelineStage, Resource::Access::ReadWrite);
    }

    for (auto& read : m_reads) {
        if (!read->exists())
            read->update();
        read->solveSynchronization(m_pipelineStage, Resource::Access::ReadOnly);
    }

    for (auto& write : m_writes) {
        if (!write->exists())
            write->update();
        write->solveSynchronization(m_pipelineStage, Resource::Access::ReadWrite);
    }

    if (Context::get().getDebugOutput())
        std::cout << "Executing { " << objectName() << " }" << std::endl;

    update();
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
