#pragma once
#include <map>

#include <vkEasy/Error.h>
#include <vkEasy/global.h>

namespace VK_EASY_NAMESPACE {
class PipelineNode;
class Device;

class ShaderStage : public Errorable {
    friend class PipelineNode;
    friend class ComputeNode;

public:
    explicit ShaderStage(const vk::ShaderStageFlagBits& stage, PipelineNode* parent);
    ShaderStage& setShaderFile(const std::string& file, bool watchForChanges = true);
    ShaderStage& setShaderData(const std::vector<char>& data);
    ShaderStage& setEntryPoint(const std::string& entryPoint);
    ShaderStage& defineConstant(uint32_t id, uint32_t offset, size_t size);
    ShaderStage& setConstantData(void* data, size_t size, bool copy = false);
    ShaderStage& clearConstants();

private:
    std::vector<char> loadShader(const std::string& fileName);
    void update(Device* device);
    vk::PipelineShaderStageCreateInfo* getPipelineShaderStageCreateInfo();

    std::unique_ptr<vk::raii::ShaderModule> m_shaderModule;
    vk::ShaderModuleCreateInfo m_moduleCreateInfo;
    vk::PipelineShaderStageCreateInfo m_shaderStageCreateInfo;
    vk::SpecializationInfo m_specializationInfo;
    std::map<uint32_t, vk::SpecializationMapEntry> m_specializationMapEntries;
    std::vector<vk::SpecializationMapEntry> m_preparedSpecializationMapEntries;
    std::vector<char> m_shaderData;
    PipelineNode* m_parent;
    std::string m_shaderEntryPoint;
    std::string m_shaderFileName;
    std::vector<char> m_data;
    bool m_watchChanges;
    bool m_shaderModuleChanged = false;
    bool m_entriesChanged = false;
};
} // namespace VK_EASY_NAMESPACE
