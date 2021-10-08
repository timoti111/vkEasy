#pragma once
#include <map>
#include <vkEasy/Node.h>
#include <vkEasy/ShaderStage.h>

namespace VK_EASY_NAMESPACE {
class PipelineNode : public Node {
    friend class ShaderStage;

public:
    void readsFrom(const vk::ArrayProxyNoTemporaries<Resource>& resources, uint32_t binding);
    void writesTo(const vk::ArrayProxyNoTemporaries<Resource>& resources, uint32_t binding);

protected:
    virtual void buildPipeline() = 0;
    ShaderStage& createShaderStage(const vk::ShaderStageFlagBits& stage);
    std::map<vk::ShaderStageFlagBits, ShaderStage> m_stages;

private:
    void needsUpdate();
};
} // namespace VK_EASY_NAMESPACE