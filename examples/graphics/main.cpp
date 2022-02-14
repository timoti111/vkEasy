#include <iostream>
#include <vkEasy/Context.h>
#include <vkEasy/Graph.h>
#include <vkEasy/nodes/GraphicsNode.h>
int main()
{
    vk::easy::Context::initialize();
    auto& device = vk::easy::Context::get().createDevice();
    auto& graph = device.createGraph();

    auto& graphics = graph.createNode<vk::easy::GraphicsNode>();

    graph.startRecording();
    graphics();
    graph.stopRecording();

    graph.run();
    device.waitForQueue();

    return 0;
}
