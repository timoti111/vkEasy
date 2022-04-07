#pragma once
#include <map>

#include <shaderc/shaderc.hpp>
#include <tuple>
#include <vkEasy/Error.h>
#include <vkEasy/global.h>

namespace VK_EASY_NAMESPACE {
class PipelineNode;
class Device;
struct Descriptor;

class ShaderStage : public Errorable {
    friend class PipelineNode;
    friend class ComputeNode;
    friend class GraphicsNode;

public:
    ShaderStage(ShaderStage const&) = delete;
    void operator=(ShaderStage const&) = delete;

    void usesDescriptor(Descriptor* descriptor);
    ShaderStage& setShaderFile(const std::string& file, bool watchForChanges = true);
    ShaderStage& setShaderData(const std::vector<uint32_t>& data);
    ShaderStage& setEntryPoint(const std::string& entryPoint);
    ShaderStage& setConstantData();
    template <typename First, typename... Rest> ShaderStage& setConstantData(const First& first, const Rest&... rest)
    {
        if (m_numberOfConstantData == 0)
            m_data.clear();
        auto ptr = reinterpret_cast<const char*>(&first);
        defineConstant(m_numberOfConstantData, m_data.size(), sizeof(First));
        m_data.insert(m_data.end(), ptr, ptr + sizeof(First));
        m_numberOfConstantData++;
        return setConstantData(rest...);
    };

private:
    explicit ShaderStage(const vk::ShaderStageFlagBits& stage, PipelineNode* parent);

    std::vector<uint32_t> loadShader(const std::string& fileName);
    std::vector<uint32_t> compileFile(
        const std::string& sourceName, shaderc_shader_kind kind, const std::string& source, bool optimize = false);
    void update(Device* device);
    vk::PipelineShaderStageCreateInfo* getPipelineShaderStageCreateInfo();
    static shaderc_shader_kind toShaderKind(vk::ShaderStageFlagBits flag);
    ShaderStage& defineConstant(uint32_t id, uint32_t offset, size_t size);

    std::unique_ptr<vk::raii::ShaderModule> m_shaderModule;
    vk::ShaderModuleCreateInfo m_moduleCreateInfo;
    vk::PipelineShaderStageCreateInfo m_shaderStageCreateInfo;
    vk::SpecializationInfo m_specializationInfo;
    std::map<uint32_t, vk::SpecializationMapEntry> m_specializationMapEntries;
    std::vector<vk::SpecializationMapEntry> m_preparedSpecializationMapEntries;
    std::vector<uint32_t> m_shaderData;
    PipelineNode* m_parent;
    std::string m_shaderEntryPoint;
    std::string m_shaderFileName;
    std::vector<char> m_data;
    bool m_watchChanges;
    bool m_shaderModuleChanged = false;
    bool m_entriesChanged = false;
    size_t m_numberOfConstantData = 0;
};
} // namespace VK_EASY_NAMESPACE
