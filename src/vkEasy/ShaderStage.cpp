#include <fstream>
#include <iostream>
#include <vkEasy/Device.h>
#include <vkEasy/ShaderStage.h>
#include <vkEasy/nodes/base/PipelineNode.h>

using namespace VK_EASY_NAMESPACE;

ShaderStage::ShaderStage(const vk::ShaderStageFlagBits& stage, PipelineNode* parent)
    : Errorable("ShaderStage")
{
    m_parent = parent;
    m_parent->needsRebuild();
    setEntryPoint("main");
    m_shaderStageCreateInfo.setStage(stage);
    m_shaderStageCreateInfo.setPSpecializationInfo(&m_specializationInfo);
}

void ShaderStage::uses(std::vector<Resource*> resources, size_t binding, size_t set)
{
    m_parent->uses(std::move(resources), binding, set, m_shaderStageCreateInfo.stage);
}

ShaderStage& ShaderStage::defineConstant(uint32_t id, uint32_t offset, size_t size)
{
    m_parent->needsRebuild();
    vk::SpecializationMapEntry entry;
    entry.setConstantID(id).setOffset(offset).setSize(size);
    m_specializationMapEntries[id] = entry;
    m_entriesChanged = true;
    return *this;
}

ShaderStage& ShaderStage::setConstantData(void* data, size_t size, bool copy)
{
    m_parent->needsRebuild();
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
    m_parent->needsRebuild();
    m_specializationMapEntries.clear();
    setConstantData(nullptr, 0);
    m_entriesChanged = true;
    return *this;
}

ShaderStage& ShaderStage::setEntryPoint(const std::string& entryPoint)
{
    m_shaderEntryPoint = entryPoint;
    m_parent->needsRebuild();
    m_shaderStageCreateInfo.setPName(m_shaderEntryPoint.c_str());
    return *this;
}

ShaderStage& ShaderStage::setShaderData(const std::vector<uint32_t>& data)
{
    m_parent->needsRebuild();
    m_shaderFileName = "";
    m_shaderData = data;
    m_moduleCreateInfo.setCodeSize(m_shaderData.size() * sizeof(uint32_t)).setPCode(m_shaderData.data());
    m_shaderModuleChanged = true;
    return *this;
}

ShaderStage& ShaderStage::setShaderFile(const std::string& file, bool watchForChanges)
{
    m_shaderFileName = file;
    m_watchChanges = watchForChanges;
    return setShaderData(loadShader(file));
}

std::vector<uint32_t> ShaderStage::compileFile(
    const std::string& sourceName, shaderc_shader_kind kind, const std::string& source, bool optimize)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    if (optimize)
        options.SetOptimizationLevel(shaderc_optimization_level_size);

    shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, kind, sourceName.c_str(), options);

    if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
        std::cerr << module.GetErrorMessage();
        return std::vector<uint32_t>();
    }

    return { module.cbegin(), module.cend() };
}

std::vector<uint32_t> ShaderStage::loadShader(const std::string& fileName)
{
    bool isSPIRV = fileName.find(".spv") != std::string::npos;
    std::ios_base::openmode openMode = std::ios::in | std::ios::ate;
    if (isSPIRV)
        openMode |= std::ios::binary;
    std::vector<uint32_t> shaderCode;

    std::ifstream is(fileName, openMode);

    if (is.is_open()) {
        size_t size = is.tellg();
        is.seekg(0, std::ios::beg);
        if (isSPIRV) {
            shaderCode.resize(size / sizeof(uint32_t));
            is.read(reinterpret_cast<char*>(shaderCode.data()), size);
        } else {
            std::string shaderText;
            shaderText.resize(size);
            is.read(shaderText.data(), size);
            shaderCode = compileFile("shader_src", toShaderKind(m_shaderStageCreateInfo.stage), shaderText);
        }
        is.close();
        assert(size > 0);
    } else {
        std::cerr << "Error: Could not open shader file \"" << fileName << "\""
                  << "\n";
    }
    return shaderCode;
}

void ShaderStage::update(Device* device)
{
    if (m_shaderModuleChanged) {
        m_shaderModule = std::make_unique<vk::raii::ShaderModule>(*device->getLogicalDevice(), m_moduleCreateInfo);
        m_shaderStageCreateInfo.setModule(**m_shaderModule);
        m_shaderModuleChanged = false;
    }
    if (m_entriesChanged) {
        m_preparedSpecializationMapEntries.clear();
        for (const auto& [key, value] : m_specializationMapEntries)
            m_preparedSpecializationMapEntries.push_back(value);
        m_specializationInfo.setMapEntries(m_preparedSpecializationMapEntries);
        m_entriesChanged = false;
    }
}

vk::PipelineShaderStageCreateInfo* ShaderStage::getPipelineShaderStageCreateInfo()
{
    return &m_shaderStageCreateInfo;
}

shaderc_shader_kind ShaderStage::toShaderKind(vk::ShaderStageFlagBits flag)
{
    switch (flag) {
    case vk::ShaderStageFlagBits::eAnyHitKHR:
        return shaderc_shader_kind::shaderc_glsl_anyhit_shader;
    case vk::ShaderStageFlagBits::eCallableKHR:
        return shaderc_shader_kind::shaderc_glsl_callable_shader;
    case vk::ShaderStageFlagBits::eClosestHitKHR:
        return shaderc_shader_kind::shaderc_glsl_closesthit_shader;
    case vk::ShaderStageFlagBits::eCompute:
        return shaderc_shader_kind::shaderc_glsl_compute_shader;
    case vk::ShaderStageFlagBits::eFragment:
        return shaderc_shader_kind::shaderc_glsl_fragment_shader;
    case vk::ShaderStageFlagBits::eGeometry:
        return shaderc_shader_kind::shaderc_glsl_geometry_shader;
    case vk::ShaderStageFlagBits::eIntersectionKHR:
        return shaderc_shader_kind::shaderc_glsl_intersection_shader;
    case vk::ShaderStageFlagBits::eMissKHR:
        return shaderc_shader_kind::shaderc_glsl_miss_shader;
    case vk::ShaderStageFlagBits::eRaygenKHR:
        return shaderc_shader_kind::shaderc_glsl_raygen_shader;
    case vk::ShaderStageFlagBits::eTessellationControl:
        return shaderc_shader_kind::shaderc_glsl_tess_control_shader;
    case vk::ShaderStageFlagBits::eTessellationEvaluation:
        return shaderc_shader_kind::shaderc_glsl_tess_evaluation_shader;
    case vk::ShaderStageFlagBits::eVertex:
        return shaderc_shader_kind::shaderc_glsl_vertex_shader;
    default:
        return shaderc_shader_kind::shaderc_glsl_infer_from_source;
    }
}