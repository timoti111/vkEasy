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
    std::vector<vk::DescriptorImageInfo> imageInfos;
    vk::ShaderStageFlags shaderStageFlags = vk::ShaderStageFlagBits::eAll;
};

class PipelineNode : public Node {
    friend class ShaderStage;
    friend class Graph;

public:
    PipelineNode(PipelineNode const&) = delete;
    void operator=(PipelineNode const&) = delete;

    Descriptor* createDescriptor(std::vector<Resource*> resources, size_t binding, size_t set);

protected:
    PipelineNode(const std::string& nodeName);
    void needsRebuild();
    virtual void buildPipeline() = 0;
    void buildLayout();
    void buildDescriptorSet();
    ShaderStage* getShaderStage(const vk::ShaderStageFlagBits& stage);

    std::map<size_t, std::map<size_t, Descriptor>> m_layout;
    std::map<vk::ShaderStageFlagBits, std::unique_ptr<ShaderStage>> m_stages;
    std::unique_ptr<vk::raii::Pipeline> m_pipeline;
    std::unique_ptr<vk::raii::PipelineCache> m_pipelineCache;
    std::vector<std::unique_ptr<vk::raii::DescriptorSetLayout>> m_setLayouts;
    std::vector<vk::DescriptorSetLayout> m_setLayoutsVk;
    std::unique_ptr<vk::raii::PipelineLayout> m_pipelineLayout;
    std::vector<vk::DescriptorPoolSize> m_poolSizes;

    std::map<size_t, std::unique_ptr<vk::raii::DescriptorPool>> m_descriptorPool;
    std::map<size_t, std::unique_ptr<vk::raii::DescriptorSets>> m_descriptorSets;
    std::map<size_t, std::vector<vk::DescriptorSet>> m_descriptorSetsToBind;
    std::map<size_t, std::vector<vk::WriteDescriptorSet>> m_writeDescriptorSets;

    std::function<void()> m_basePipelineUpdateFunction;
    bool m_pipelineRebuild = true;
    bool m_pipelineLayoutRebuild = true;
};
} // namespace VK_EASY_NAMESPACE