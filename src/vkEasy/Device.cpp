#include <vkEasy/Device.h>

using namespace VK_EASY_NAMESPACE;

Device::Device(vk::raii::PhysicalDevice* device)
    : Errorable("Device")
{
}

Graph* Device::createGraph()
{
    m_graphs.push_back(std::unique_ptr<Graph>(new Graph()));
    m_graphs.back()->setParent(this);
    return m_graphs.back().get();
}

vk::raii::Device* Device::getVkDevice()
{
}

void Device::findDevice()
{
}

void Device::initialize()
{
}