#include <algorithm>
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

    if (Context::get().getDebugOutput()) {
        context.addLayer("VK_LAYER_KHRONOS_validation");
        context.addExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    context.addExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);

    std::vector<std::string> unsupportedExtensions(context.m_extensions.size());
    auto it = std::set_difference(context.m_extensions.begin(), context.m_extensions.end(),
        context.m_supportedExtensions.begin(), context.m_supportedExtensions.end(), unsupportedExtensions.begin());
    unsupportedExtensions.resize(it - unsupportedExtensions.begin());

    std::vector<std::string> unsupportedLayers(context.m_layers.size());
    it = std::set_difference(context.m_layers.begin(), context.m_layers.end(), context.m_supportedLayers.begin(),
        context.m_supportedLayers.end(), unsupportedLayers.begin());
    unsupportedLayers.resize(it - unsupportedLayers.begin());

    std::for_each(unsupportedExtensions.begin(), unsupportedExtensions.end(),
        [](const auto& ext) { std::cout << "Unsupported instance extension: " << ext << std::endl; });
    std::for_each(unsupportedLayers.begin(), unsupportedLayers.end(),
        [](const auto& ext) { std::cout << "Unsupported instance layer: " << ext << std::endl; });
    if (!unsupportedExtensions.empty() || !unsupportedLayers.empty())
        context.error(Error::RequirementsNotFulfilled);

    std::vector<char const*> extensions;
    std::transform(context.m_extensions.begin(), context.m_extensions.end(), std::back_inserter(extensions),
        [](const auto& string) -> char const* { return string.c_str(); });

    std::vector<char const*> layers;
    std::transform(context.m_layers.begin(), context.m_layers.end(), std::back_inserter(layers),
        [](const auto& string) -> char const* { return string.c_str(); });

    context.m_instanceCreateInfo.setPApplicationInfo(&context.m_applicationInfo)
        .setPEnabledExtensionNames(extensions)
        .setPEnabledLayerNames(layers);

    if (context.m_extensions.contains(VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
        vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
            /*| vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo*/
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning);
        vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
            | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

        context.m_debugMessengerCreateInfo.setMessageSeverity(severityFlags)
            .setMessageType(messageTypeFlags)
            .setPfnUserCallback(debugUtilsMessengerCallback);

        context.m_instanceCreateInfo.setPNext(&context.m_debugMessengerCreateInfo);
    }

    context.m_instance = std::make_unique<vk::raii::Instance>(*context.m_context, context.m_instanceCreateInfo);

    if (context.m_extensions.contains(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        context.m_debugMessenger = std::make_unique<vk::raii::DebugUtilsMessengerEXT>(
            *context.m_instance, context.m_debugMessengerCreateInfo);

    if (Context::get().getDebugOutput()) {
        auto version = context.m_instanceCreateInfo.pApplicationInfo->apiVersion;
        std::cout << "Vulkan instance initialised with: " << std::endl;
        std::cout << "Version: " << VK_VERSION_MAJOR(version) << "." << VK_VERSION_MINOR(version) << "."
                  << VK_VERSION_PATCH(version) << std::endl;
        std::for_each(context.m_layers.begin(), context.m_layers.end(),
            [](const auto& ext) { std::cout << "Instance Layer: " << ext << std::endl; });
        std::for_each(context.m_extensions.begin(), context.m_extensions.end(),
            [](const auto& ext) { std::cout << "Instance Extension: " << ext << std::endl; });
        std::cout << std::endl;
    }

    context.m_physicalDevices = std::make_unique<vk::raii::PhysicalDevices>(*context.m_instance);
}

Context::Context()
    : Errorable("Context")
{
    m_context = std::make_unique<vk::raii::Context>();
    m_applicationInfo.setApiVersion(m_context->enumerateInstanceVersion())
        .setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
        .setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
        .setPApplicationName("vkEasy")
        .setPEngineName("vkEasy");

    auto extensionProperties = m_context->enumerateInstanceExtensionProperties();
    std::transform(extensionProperties.begin(), extensionProperties.end(),
        std::inserter(m_supportedExtensions, m_supportedExtensions.end()),
        [](const auto& ext) -> std::string { return std::string(ext.extensionName.data()); });
    auto layerProperties = m_context->enumerateInstanceLayerProperties();
    std::transform(layerProperties.begin(), layerProperties.end(),
        std::inserter(m_supportedLayers, m_supportedLayers.end()),
        [](const auto& layer) -> std::string { return std::string(layer.layerName.data()); });
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
        error(Error::CreationInfoModifyAfterInitialization);
    m_applicationInfo = applicationInfo;
}

void Context::addExtension(const std::string& extension)
{
    if (m_instance)
        error(Error::CreationInfoModifyAfterInitialization);
    m_extensions.insert(extension);
    // std::vector<vk::ExtensionProperties> extensionProperties = m_context->enumerateInstanceExtensionProperties();
    // if (std::find_if(extensionProperties.begin(), extensionProperties.end(),
    //         [&extension](const auto& ep) { return (strcmp(extension.c_str(), ep.extensionName) == 0); })
    //     != extensionProperties.end()) {
    //     m_extensions.insert(extension);
    // }
}

void Context::addExtensions(const std::vector<std::string>& extensions)
{
    if (m_instance)
        error(Error::CreationInfoModifyAfterInitialization);
    std::for_each(extensions.begin(), extensions.end(), [this](const auto& extension) { addExtension(extension); });
}

void Context::addLayer(const std::string& layer)
{

    if (m_instance)
        error(Error::CreationInfoModifyAfterInitialization);
    m_layers.insert(layer);
    // std::vector<vk::LayerProperties> layerProperties = m_context->enumerateInstanceLayerProperties();
    // if (std::find_if(layerProperties.begin(), layerProperties.end(),
    //         [&layer](const auto& lp) { return (strcmp(layer.c_str(), lp.layerName) == 0); })
    //     != layerProperties.end()) {
    //     m_layers.insert(layer);
    // }
}

void Context::addLayers(const std::vector<std::string>& layers)
{
    if (m_instance)
        error(Error::CreationInfoModifyAfterInitialization);
    std::for_each(layers.begin(), layers.end(), [this](const auto& layer) { addLayer(layer); });
}

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

vk::raii::PhysicalDevices& Context::getPhysicalDevices()
{
    return *m_physicalDevices;
}

Device& Context::createDevice(size_t index)
{
    m_devices.push_back(std::unique_ptr<Device>(new Device(index)));
    return *m_devices.back().get();
}

vk::raii::Instance& Context::instance()
{
    return *m_instance;
}

void Context::setDebugOutput(bool debug)
{
    m_debugOutput = debug;
}

bool Context::getDebugOutput()
{
    return m_debugOutput;
}
