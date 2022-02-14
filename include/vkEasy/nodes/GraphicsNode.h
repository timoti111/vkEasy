#pragma once
#include <vkEasy/nodes/base/PipelineNode.h>

namespace VK_EASY_NAMESPACE {
class GraphicsNode : public PipelineNode {
    friend class Graph;

public:
    GraphicsNode(GraphicsNode const&) = delete;
    void operator=(GraphicsNode const&) = delete;

protected:
    GraphicsNode();
    void update(Device* device);
    void buildPipeline(vk::easy::Device* device);

private:
    vk::GraphicsPipelineCreateInfo m_graphicsPipelineCreateInfo;
    std::unique_ptr<vk::raii::PipelineCache> m_pipelineCache;
};
} // namespace VK_EASY_NAMESPACE