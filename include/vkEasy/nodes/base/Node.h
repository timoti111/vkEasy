#pragma once
#include <set>
#include <vkEasy/Error.h>
#include <vkEasy/Utils.h>
#include <vkEasy/global.h>
#include <vkEasy/resources/base/Resource.h>

namespace VK_EASY_NAMESPACE {
class Graph;
class Device;
class Buffer;
class Image;
class Node : public Errorable, public Object {
    friend class Graph;
    friend class Device;
    friend class Buffer;
    friend class Image;

public:
    Node(Node const&) = delete;
    void operator=(Node const&) = delete;
    virtual ~Node() = default;

    void needsExtensions(const std::initializer_list<std::string>& extensions);
    void setCullImmune(bool cullImmune);

protected:
    explicit Node(const std::string& nodeName);

    void execute();
    void uses(Resource* resource, Resource::Access access = Resource::Access::ReadWrite);

    void addExecutionBarrier(vk::PipelineStageFlagBits src, vk::PipelineStageFlagBits dst);
    void addBufferBarrier(vk::PipelineStageFlags src, vk::PipelineStageFlags dst, vk::Buffer buffer,
        vk::AccessFlagBits srcMask, vk::AccessFlagBits dstMask);
    virtual void update() = 0;
    std::vector<vk::raii::CommandBuffer*> getCommandBuffers(size_t count);

    vk::QueueFlagBits m_neededQueueType;
    vk::PipelineStageFlagBits m_pipelineStage = vk::PipelineStageFlagBits::eNoneKHR;

private:
    std::set<std::string> m_neededExtensions;
    std::set<std::string> m_optionalExtensions;

    std::set<Resource*> m_creates;
    std::set<Resource*> m_reads;
    std::set<Resource*> m_writes;
    bool m_cullImmune = false;
    size_t m_referenceCount;
};
} // namespace VK_EASY_NAMESPACE
