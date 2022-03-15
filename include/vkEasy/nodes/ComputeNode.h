#pragma once
#include <map>
#include <vkEasy/nodes/base/PipelineNode.h>

namespace VK_EASY_NAMESPACE {
class ComputeNode : public PipelineNode {
    friend class Graph;

public:
    ComputeNode(ComputeNode const&) = delete;
    void operator=(ComputeNode const&) = delete;

    ShaderStage& getComputeShaderStage();
    void setDispatchSize(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);

protected:
    ComputeNode();
    void update(Device* device);
    void buildPipeline(vk::easy::Device* device);

private:
    vk::ComputePipelineCreateInfo m_computePipelineCreateInfo;
    std::array<size_t, 3> m_dispatchSize;
};
} // namespace VK_EASY_NAMESPACE