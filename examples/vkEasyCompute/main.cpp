#include <iostream>
#include <vkEasy/Context.h>
#include <vkEasy/Graph.h>
#include <vkEasy/nodes/ComputeNode.h>
#include <vkEasy/nodes/MemoryReadNode.h>
#include <vkEasy/nodes/MemoryWriteNode.h>
#include <vkEasy/resources/StagingBuffer.h>
#include <vkEasy/resources/StorageBuffer.h>

#define BUFFER_ELEMENTS 32
struct SpecializationData {
    uint32_t BUFFER_ELEMENT_COUNT = BUFFER_ELEMENTS;
};

int main()
{
    // Setup phase
    auto& device = vk::easy::Context::get().createDevice();
    auto& graph = device.createGraph();

    const size_t bufferSize = BUFFER_ELEMENTS * sizeof(uint32_t);

    std::vector<uint32_t> computeInput(BUFFER_ELEMENTS);
    uint32_t n = 0;
    std::generate(computeInput.begin(), computeInput.end(), [&n] { return n++; });

    auto dataPtr8 = reinterpret_cast<uint8_t*>(computeInput.data());
    std::vector<uint8_t> computeInputBytes(dataPtr8, dataPtr8 + bufferSize);

    auto& gpuBuffer = graph.createStorageBuffer();
    gpuBuffer.setSize(bufferSize);

    auto& memoryWrite = graph.createMemoryWriteNode();
    memoryWrite.setDstResource(gpuBuffer);
    memoryWrite.setData(computeInputBytes);

    auto& compute = graph.createComputeNode();
    auto gpuBufferDescriptor = compute.createDescriptor({ &gpuBuffer }, 0, 0);
    compute.setDispatchSize(BUFFER_ELEMENTS, 1, 1);

    SpecializationData specializationData;
    auto& stage = compute.getComputeShaderStage();
    stage.setShaderFile("headless.comp");
    stage.defineConstant(
        0, offsetof(SpecializationData, BUFFER_ELEMENT_COUNT), sizeof(SpecializationData::BUFFER_ELEMENT_COUNT));
    stage.setConstantData(&specializationData, sizeof(SpecializationData), true);
    stage.usesDescriptor(gpuBufferDescriptor);

    std::vector<uint32_t> computeOutput;
    auto& memoryRead = graph.createMemoryReadNode();
    memoryRead.setSrcResource(gpuBuffer);
    memoryRead.onDataReady([&computeOutput](auto& data) {
        auto dataPtr32 = reinterpret_cast<const uint32_t*>(data.data());
        computeOutput.insert(computeOutput.end(), dataPtr32, dataPtr32 + BUFFER_ELEMENTS);
    });

    graph.enqueueNode(memoryWrite);
    graph.enqueueNode(compute);
    graph.enqueueNode(memoryRead);

    // Compile phase
    graph.compile();

    // Execute phase
    graph.execute();

    std::cout << "Compute input:" << std::endl;
    for (auto& v : computeInput)
        std::cout << v << '\t';
    std::cout << std::endl;

    std::cout << "Compute output:" << std::endl;
    for (auto& v : computeOutput)
        std::cout << v << '\t';
    std::cout << std::endl;

    return 0;
}
