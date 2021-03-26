#include <iostream>
#include <vkEasy/Context.h>
using namespace VK_EASY_NAMESPACE;

Context& Context::get()
{
    static Context context;
    return context;
}

void Context::initialize()
{
    auto& context = Context::get();
    if (context.m_instance)
        return;

    std::vector<char const*> extensions;
    std::transform(context.m_extensions.begin(), context.m_extensions.end(), std::back_inserter(extensions),
        [](const std::string& string) -> char const* { return string.c_str(); });

    std::vector<char const*> layers;
    std::transform(context.m_layers.begin(), context.m_layers.end(), std::back_inserter(layers),
        [](const std::string& string) -> char const* { return string.c_str(); });

    vk::InstanceCreateInfo info;
    info.setPApplicationInfo(&context.m_applicationInfo)
        .setPEnabledExtensionNames(extensions)
        .setPEnabledLayerNames(layers);
    context.m_instance = std::make_unique<vk::raii::Instance>(*context.m_context, info);

#ifndef NDEBUG
    auto version = info.pApplicationInfo->apiVersion;
    std::cout << "Vulkan instance initialised with: " << std::endl;
    std::cout << "Version: " << VK_VERSION_MAJOR(version) << "." << VK_VERSION_MINOR(version) << "."
              << VK_VERSION_PATCH(version) << std::endl;
    std::for_each(context.m_extensions.begin(), context.m_extensions.end(),
        [](const std::string& ext) { std::cout << "Extension: " << ext << std::endl; });
    std::for_each(context.m_layers.begin(), context.m_layers.end(),
        [](const std::string& ext) { std::cout << "Layer: " << ext << std::endl; });

    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
        | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning);
    vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
        | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

    vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo;
    debugUtilsMessengerCreateInfo.setMessageSeverity(severityFlags)
        .setMessageType(messageTypeFlags)
        .setPfnUserCallback(debugUtilsMessengerCallback);
    context.m_debugMessenger
        = std::make_unique<vk::raii::DebugUtilsMessengerEXT>(*context.m_instance, debugUtilsMessengerCreateInfo);
#endif
}

Context::Context()
{
    m_context = std::make_unique<vk::raii::Context>();
    m_applicationInfo.setApiVersion(m_context->enumerateInstanceVersion())
        .setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
        .setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
        .setPApplicationName("vkEasy")
        .setPEngineName("vkEasy");

#ifndef NDEBUG
    std::vector<vk::LayerProperties> layerProperties = m_context->enumerateInstanceLayerProperties();
    std::vector<vk::ExtensionProperties> extensionProperties = m_context->enumerateInstanceExtensionProperties();
    if (std::find_if(layerProperties.begin(), layerProperties.end(),
            [](vk::LayerProperties const& lp) { return (strcmp("VK_LAYER_KHRONOS_validation", lp.layerName) == 0); })
        != layerProperties.end()) {
        m_layers.insert("VK_LAYER_KHRONOS_validation");
    }
    if (std::find_if(layerProperties.begin(), layerProperties.end(),
            [](vk::LayerProperties const& lp) {
                return (strcmp("VK_LAYER_LUNARG_assistant_layer", lp.layerName) == 0);
            })
        != layerProperties.end()) {
        m_layers.insert("VK_LAYER_LUNARG_assistant_layer");
    }
    if (std::find_if(extensionProperties.begin(), extensionProperties.end(),
            [](vk::ExtensionProperties const& ep) {
                return (strcmp(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, ep.extensionName) == 0);
            })
        != extensionProperties.end()) {
        m_extensions.insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
#endif
}

std::set<std::string> Context::extensions() const
{
    return m_extensions;
}

std::set<std::string> Context::layers() const
{
    return m_layers;
}

vk::ApplicationInfo Context::applicationInfo() const
{
    return m_applicationInfo;
}

void Context::setApplicationInfo(const vk::ApplicationInfo& applicationInfo)
{
    if (m_instance)
        return;
    m_applicationInfo = applicationInfo;
}

void Context::addExtension(const std::string& extension)
{
    if (m_instance)
        return;
    m_extensions.insert(extension);
}

void Context::addExtensions(std::vector<std::string> extensions)
{
    if (m_instance)
        return;
    std::copy(extensions.begin(), extensions.end(), std::inserter(m_extensions, m_extensions.end()));
}

void Context::addLayer(const std::string& layer)
{
    if (m_instance)
        return;
    m_layers.insert(layer);
}

void Context::addLayers(std::vector<std::string> layers)
{
    if (m_instance)
        return;
    std::copy(layers.begin(), layers.end(), std::inserter(m_layers, m_layers.end()));
}

#ifndef NDEBUG
VkBool32 Context::debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
{
    std::cerr << vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)) << ": "
              << vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageTypes)) << ":\n";
    std::cerr << "\t"
              << "messageIDName   = <" << pCallbackData->pMessageIdName << ">\n";
    std::cerr << "\t"
              << "messageIdNumber = " << pCallbackData->messageIdNumber << "\n";
    std::cerr << "\t"
              << "message         = <" << pCallbackData->pMessage << ">\n";
    if (0 < pCallbackData->queueLabelCount) {
        std::cerr << "\t"
                  << "Queue Labels:\n";
        for (uint8_t i = 0; i < pCallbackData->queueLabelCount; i++) {
            std::cerr << "\t\t"
                      << "labelName = <" << pCallbackData->pQueueLabels[i].pLabelName << ">\n";
        }
    }
    if (0 < pCallbackData->cmdBufLabelCount) {
        std::cerr << "\t"
                  << "CommandBuffer Labels:\n";
        for (uint8_t i = 0; i < pCallbackData->cmdBufLabelCount; i++) {
            std::cerr << "\t\t"
                      << "labelName = <" << pCallbackData->pCmdBufLabels[i].pLabelName << ">\n";
        }
    }
    if (0 < pCallbackData->objectCount) {
        std::cerr << "\t"
                  << "Objects:\n";
        for (uint8_t i = 0; i < pCallbackData->objectCount; i++) {
            std::cerr << "\t\t"
                      << "Object " << i << "\n";
            std::cerr << "\t\t\t"
                      << "objectType   = "
                      << vk::to_string(static_cast<vk::ObjectType>(pCallbackData->pObjects[i].objectType)) << "\n";
            std::cerr << "\t\t\t"
                      << "objectHandle = " << pCallbackData->pObjects[i].objectHandle << "\n";
            if (pCallbackData->pObjects[i].pObjectName) {
                std::cerr << "\t\t\t"
                          << "objectName   = <" << pCallbackData->pObjects[i].pObjectName << ">\n";
            }
        }
    }
    return VK_TRUE;
}
#endif
