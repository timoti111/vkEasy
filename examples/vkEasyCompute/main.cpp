#include <iostream>
#include <vkEasy/vkEasy.h>

const uint32_t BUFFER_ELEMENTS = 32;

int main()
{
    // Setup phase
    vk::easy::Context::get().setDebugOutput(true);
    auto& device = vk::easy::Context::get().createDevice();
    auto& graph = device.createGraph();

    std::vector<uint32_t> computeInput(BUFFER_ELEMENTS);
    uint32_t n = 0;
    std::generate(computeInput.begin(), computeInput.end(), [&n] { return n++; });

    auto& gpuBuffer = graph.createStorageBuffer();
    gpuBuffer.setData(computeInput);
    gpuBuffer.setDataToRead();

    auto& compute = graph.createComputeNode();
    auto gpuBufferDescriptor = compute.createDescriptor({ &gpuBuffer }, 0, 0);
    compute.setDispatchSize(BUFFER_ELEMENTS, 1, 1);
    compute.setCullImmune(true);

    auto& stage = compute.getComputeShaderStage();
    stage.setShaderFile("headless.comp");
    stage.setConstantData(BUFFER_ELEMENTS);
    stage.usesDescriptor(gpuBufferDescriptor);

    graph.enqueueNode(compute);

    // Compile phase
    graph.compile();

    // Execute phase
    graph.execute();

    std::cout << "Compute input:" << std::endl;
    for (auto& v : computeInput)
        std::cout << v << '\t';
    std::cout << std::endl;

    auto computeOutput = gpuBuffer.getData<uint32_t>();
    std::cout << "Compute output:" << std::endl;
    for (auto& v : computeOutput)
        std::cout << v << '\t';
    std::cout << std::endl;

    return 0;
}
