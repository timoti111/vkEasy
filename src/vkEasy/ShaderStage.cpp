#include <fstream>
#include <iostream>
#include <vkEasy/ShaderStage.h>
#include <vkEasy/nodes/PipelineNode.h>

using namespace VK_EASY_NAMESPACE;

ShaderStage::ShaderStage(const vk::ShaderStageFlagBits& stage, PipelineNode* parent)
{
    m_parent = parent;
    m_parent->needsUpdate();
    setEntryPoint("main");
    m_shaderStageCreateInfo.setStage(stage);
    m_shaderStageCreateInfo.setPSpecializationInfo(&m_specializationInfo);
}

ShaderStage& ShaderStage::defineConstant(uint32_t id, uint32_t offset, size_t size)
{
    m_parent->needsUpdate();
    vk::SpecializationMapEntry entry;
    entry.setConstantID(id).setOffset(offset).setSize(size);
    m_specializationMapEntries[id] = entry;
    m_entriesChanged = true;
    return *this;
}

ShaderStage& ShaderStage::setConstantData(void* data, size_t size, bool copy)
{
    m_parent->needsUpdate();
    void* dataPtr = data;
    if (copy) {
        m_data = std::vector<char>(size);
        std::memcpy(m_data.data(), data, size);
        dataPtr = m_data.data();
    }
    m_specializationInfo.setDataSize(size).setPData(dataPtr);
    return *this;
}

ShaderStage& ShaderStage::clearConstants()
{
    m_parent->needsUpdate();
    m_specializationMapEntries.clear();
    setConstantData(nullptr, 0);
    m_entriesChanged = true;
    return *this;
}

ShaderStage& ShaderStage::setEntryPoint(const std::string& entryPoint)
{
    m_parent->needsUpdate();
    m_shaderStageCreateInfo.setPName(entryPoint.c_str());
    return *this;
}

ShaderStage& ShaderStage::setShaderData(const std::vector<char>& data)
{
    m_parent->needsUpdate();
    m_shaderFileName = "";
    m_shaderData = data;
    m_moduleCreateInfo.setCodeSize(m_shaderData.size()).setPCode((uint32_t*)m_shaderData.data());
    m_shaderModuleChanged = true;
    return *this;
}

ShaderStage& ShaderStage::setShaderFile(const std::string& file, bool watchForChanges)
{
    m_shaderFileName = file;
    m_watchChanges = watchForChanges;
    return setShaderData(loadShader(file));
}

std::vector<char> ShaderStage::loadShader(const std::string& fileName)
{
    // TODO determine if SPIR-V or GLSL
    std::ifstream is(fileName, std::ios::binary | std::ios::in | std::ios::ate);

    if (is.is_open()) {
        size_t size = is.tellg();
        is.seekg(0, std::ios::beg);
        std::vector<char> shaderCode(size);
        is.read(shaderCode.data(), size);
        is.close();
        assert(size > 0);
        return shaderCode;
    } else {
        std::cerr << "Error: Could not open shader file \"" << fileName << "\""
                  << "\n";
        return std::vector<char>();
    }
}

vk::PipelineShaderStageCreateInfo& ShaderStage::update(vk::raii::Device& device)
{
    if (m_shaderModuleChanged) {
        m_shaderModule = std::make_unique<vk::raii::ShaderModule>(device, m_moduleCreateInfo);
        m_shaderStageCreateInfo.setModule(**m_shaderModule);
    }
    if (m_entriesChanged) {
        m_preparedSpecializationMapEntries.clear();
        for (const auto& [key, value] : m_specializationMapEntries)
            m_preparedSpecializationMapEntries.push_back(value);
        m_specializationInfo.setMapEntries(m_preparedSpecializationMapEntries);
    }
    return m_shaderStageCreateInfo;
}