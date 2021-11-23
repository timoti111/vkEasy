#include <iostream>
#include <thread>
#include <vkEasy/Context.h>
#include <vkEasy/Graph.h>
#include <vkEasy/nodes/BufferCopyNode.h>
#include <vkEasy/nodes/ComputeNode.h>
#include <vkEasy/resources/StagingBuffer.h>
#include <vkEasy/resources/StorageBuffer.h>

#define BUFFER_ELEMENTS 32
struct SpecializationData {
    uint32_t BUFFER_ELEMENT_COUNT = BUFFER_ELEMENTS;
};

int main()
{
    try {
        vk::easy::Context::initialize();
        auto device = vk::easy::Context::get().createDevice();
        auto graph = device->createGraph();

        const size_t bufferSize = BUFFER_ELEMENTS * sizeof(uint32_t);

        std::vector<uint32_t> computeInput(BUFFER_ELEMENTS);
        std::vector<uint32_t> computeOutput(BUFFER_ELEMENTS);
        uint32_t n = 0;
        std::generate(computeInput.begin(), computeInput.end(), [&n] { return n++; });

        auto cpuBuffer = graph->createResource<vk::easy::StagingBuffer>();
        cpuBuffer->setData(computeInput);

        auto gpuBuffer = graph->createResource<vk::easy::StorageBuffer>();
        gpuBuffer->setSize(bufferSize);

        auto cpuToGpu = graph->createNode<vk::easy::BufferCopyNode>();
        cpuToGpu->setSrcBuffer(cpuBuffer);
        cpuToGpu->setDstBuffer(gpuBuffer);

        auto compute = graph->createNode<vk::easy::ComputeNode>();
        auto stage = compute->getShaderStage();
        SpecializationData specializationData;
        stage->setShaderFile("headless.comp.spv");
        stage->defineConstant(
            0, offsetof(SpecializationData, BUFFER_ELEMENT_COUNT), sizeof(SpecializationData::BUFFER_ELEMENT_COUNT));
        stage->setConstantData(&specializationData, sizeof(SpecializationData), true);
        stage->uses({ gpuBuffer }, 0, 0);
        compute->setDispatchSize(BUFFER_ELEMENTS, 1, 1);

        auto gpuToCpu = graph->createNode<vk::easy::BufferCopyNode>();
        gpuToCpu->setSrcBuffer(gpuBuffer);
        gpuToCpu->setDstBuffer(cpuBuffer);

        graph->startRecording();
        (*cpuToGpu)();
        (*compute)();
        (*gpuToCpu)();
        graph->stopRecording();

        graph->run();

        cpuBuffer->getData(computeOutput);

        std::cout << "Compute input:" << std::endl;
        for (auto& v : computeInput)
            std::cout << v << '\t';
        std::cout << std::endl;

        std::cout << "Compute output:" << std::endl;
        for (auto& v : computeOutput)
            std::cout << v << '\t';
        std::cout << std::endl;

    } catch (vk::SystemError& err) {
        std::cout << "vk::SystemError: " << err.what() << std::endl;
        exit(-1);
    } catch (vk::easy::Exception& err) {
        std::cout << "vk::easy:Exception: " << err.what() << std::endl;
        exit(-1);
    } catch (std::exception& err) {
        std::cout << "std::exception: " << err.what() << std::endl;
        exit(-1);
    } catch (...) {
        std::cout << "unknown error\n";
        exit(-1);
    }
    return 0;
}
