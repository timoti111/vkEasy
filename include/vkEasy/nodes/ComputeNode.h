#pragma once
#include <map>
#include <vkEasy/nodes/base/PipelineNode.h>

namespace VK_EASY_NAMESPACE {
class ComputeNode : public PipelineNode {
    friend class Graph;

public:
    ComputeNode(ComputeNode const&) = delete;
    void operator=(ComputeNode const&) = delete;

    ShaderStage* getShaderStage();
    void onUpdate(std::function<void(ComputeNode&)> update);
    void setDispatchSize(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);

protected:
    ComputeNode();
    virtual void buildPipeline(vk::easy::Device* device) override;

private:
    ShaderStage* m_shaderStage;
    vk::ComputePipelineCreateInfo m_computePipelineCreateInfo;
    std::unique_ptr<vk::raii::PipelineCache> m_pipelineCache;
    std::array<size_t, 3> m_dispatchSize;
};
} // namespace VK_EASY_NAMESPACE