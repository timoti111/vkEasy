#include <algorithm>
#include <iostream>
#include <thread>
#include <vkEasy/Context.h>
#include <vkEasy/Graph.h>

using namespace VK_EASY_NAMESPACE;

Graph::Graph()
    : Errorable("Graph")
{
}

void Graph::startRecording()
{
    m_nodeOrderGraph.clear();
    m_resourceUsage.clear();
    m_callGraph.clear();
    m_recording = true;
}

void Graph::stopRecording()
{
    m_recording = false;
    auto resourceUsageCopy = m_resourceUsage;
    for (auto& node : m_nodeOrderGraph)
        m_callGraph.push_back([=]() { node->execute(); });
    m_device->initialize();
}

void Graph::run()
{
    if (m_recording)
        error(Error::RecordingGraph);

    m_events.clear();
    setActualPipelineStage(vk::PipelineStageFlagBits::eNoneKHR);

    for (auto& action : m_callGraph)
        action();
    m_device->sendCommandBuffers();

    for (auto& event : m_events) {
        while (event.vkEvent->getStatus() != vk::Result::eEventSet)
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        event.action();
    }

    m_device->waitForFences();
    m_device->resetCommandBuffers();
}

void Graph::setDevice(Device* device)
{
    if (m_device == device)
        return;
    m_device = device;
    for (auto& node : m_nodes)
        node->setGraph(this);
}

vk::raii::Event* Graph::createEvent(std::function<void()> action)
{
    vk::EventCreateInfo info;
    m_events.push_back(Event());
    m_events.back().vkEvent = std::make_unique<vk::raii::Event>(*m_device->getLogicalDevice(), info);
    m_events.back().action = action;
    return m_events.back().vkEvent.get();
}

vk::PipelineStageFlagBits Graph::getLastPipelineStage()
{
    return m_lastPipelineStage;
}

void Graph::setActualPipelineStage(vk::PipelineStageFlagBits stage)
{
    m_lastPipelineStage = stage;
}