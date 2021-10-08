#pragma once
#include <map>
#include <vkEasy/nodes/PipelineNode.h>

namespace VK_EASY_NAMESPACE {
class ComputeNode : public PipelineNode {
public:
    ShaderStage* getShaderStage();
    void onUpdate(std::function<void(ComputeNode&)> update);
    void setDispatchSize(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ);

protected:
    virtual void buildPipeline() override;
};
} // namespace VK_EASY_NAMESPACE