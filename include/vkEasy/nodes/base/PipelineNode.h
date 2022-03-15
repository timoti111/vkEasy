#pragma once
#include <map>
#include <vkEasy/ShaderStage.h>
#include <vkEasy/nodes/base/Node.h>

namespace VK_EASY_NAMESPACE {
struct Descriptor {
    friend class ShaderStage;
    friend class PipelineNode;

private:
    vk::DescriptorType type;
    std::vector<Resource*> resources;
    std::vector<vk::DescriptorBufferInfo> bufferInfos;
    vk::ShaderStageFlags shaderStageFlags;
};

class PipelineNode : public Node {
    friend class ShaderStage;

public:
    PipelineNode(PipelineNode const&) = delete;
    void operator=(PipelineNode const&) = delete;

    Descriptor* createDescriptor(std::vector<Resource*> resources, size_t binding, size_t set);

protected:
    PipelineNode(const std::string& nodeName);
    void needsRebuild();
    virtual void buildPipeline(vk::easy::Device* device) = 0;
    void build(Device* device);
    ShaderStage* getShaderStage(const vk::ShaderStageFlagBits& stage);

    std::map<vk::ShaderStageFlagBits, std::unique_ptr<ShaderStage>> m_stages;
    std::unique_ptr<vk::raii::Pipeline> m_pipeline;
    std::unique_ptr<vk::raii::PipelineCache> m_pipelineCache;
    std::unique_ptr<vk::raii::DescriptorPool> m_descriptorPool;
    std::vector<std::unique_ptr<vk::raii::DescriptorSetLayout>> m_setLayouts;
    std::unique_ptr<vk::raii::DescriptorSets> m_descriptorSets;
    std::vector<vk::DescriptorSet> m_descriptorSetsToBind;
    std::vector<vk::DescriptorSetLayout> m_setLayoutsVk;
    std::unique_ptr<vk::raii::PipelineLayout> m_pipelineLayout;

    std::function<void(Device*)> m_basePipelineUpdateFunction;
    bool m_needsRebuild = true;
    bool m_pipelineRebuild = false;
    std::map<size_t, std::map<size_t, Descriptor>> m_layout;
    std::vector<vk::DescriptorPoolSize> m_poolSizes;
    std::vector<vk::WriteDescriptorSet> m_writeDescriptorSets;
};
} // namespace VK_EASY_NAMESPACE