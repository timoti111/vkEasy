#pragma once
#include <map>
#include <vkEasy/ShaderStage.h>
#include <vkEasy/nodes/base/Node.h>

namespace VK_EASY_NAMESPACE {
class PipelineNode : public Node {
    friend class ShaderStage;

public:
    PipelineNode(PipelineNode const&) = delete;
    void operator=(PipelineNode const&) = delete;

protected:
    PipelineNode(const std::string& nodeName);
    void needsRebuild();
    virtual void buildPipeline(vk::raii::Device* device) = 0;
    ShaderStage& createShaderStage(const vk::ShaderStageFlagBits& stage);

    std::map<vk::ShaderStageFlagBits, ShaderStage> m_stages;
    std::unique_ptr<vk::raii::Pipeline> m_pipeline;
    std::unique_ptr<vk::raii::PipelineLayout> m_pipelineLayout;
    std::function<void()> m_onUpdateFunction;

private:
    bool m_needsRebuild = true;
};
} // namespace VK_EASY_NAMESPACE