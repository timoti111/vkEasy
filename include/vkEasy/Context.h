#pragma once
#include <set>
#include <vkEasy/Device.h>
#include <vkEasy/Error.h>
#include <vkEasy/global.h>

namespace VK_EASY_NAMESPACE {
class Context : public Errorable {
    friend class Device;

public:
    Context(Context const&) = delete;
    void operator=(Context const&) = delete;

    static Context& get();
    vk::raii::Instance& instance();
    std::set<std::string> extensions() const;
    std::set<std::string> layers() const;
    vk::ApplicationInfo applicationInfo() const;
    void setApplicationInfo(const vk::ApplicationInfo& applicationInfo);
    void addExtension(const std::string& extension);
    void addExtensions(const std::vector<std::string>& extensions);
    void addLayer(const std::string& layer);
    void addLayers(const std::vector<std::string>& layers);
    void setDebugOutput(bool debug);
    bool getDebugOutput();

    vk::raii::PhysicalDevices& getPhysicalDevices();
    vk::easy::Device& createDevice(size_t index = ~0);

private:
    Context();
    static void initialize();

    std::set<std::string> m_extensions;
    std::set<std::string> m_supportedExtensions;
    std::set<std::string> m_layers;
    std::set<std::string> m_supportedLayers;
    vk::ApplicationInfo m_applicationInfo;
    vk::InstanceCreateInfo m_instanceCreateInfo;
    vk::DebugUtilsMessengerCreateInfoEXT m_debugMessengerCreateInfo;

    std::unique_ptr<vk::raii::Context> m_context;
    std::unique_ptr<vk::raii::Instance> m_instance;
    std::unique_ptr<vk::raii::DebugUtilsMessengerEXT> m_debugMessenger;
    std::unique_ptr<vk::raii::PhysicalDevices> m_physicalDevices;
    std::vector<std::unique_ptr<Device>> m_devices;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData, void*);

    bool m_debugOutput = false;
};
} // namespace VK_EASY_NAMESPACE
