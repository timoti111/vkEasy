#include <iostream>
#include <vkEasy/Context.h>
#include <vkEasy/Graph.h>
#include <vkEasy/nodes/ComputeNode.h>
#include <vkEasy/nodes/ResourceReadNode.h>
#include <vkEasy/nodes/ResourceWriteNode.h>
#include <vkEasy/resources/Buffer.h>

#define BUFFER_ELEMENTS 32
struct SpecializationData {
    uint32_t BUFFER_ELEMENT_COUNT = BUFFER_ELEMENTS;
};

int main()
{
    try {
        vk::easy::Context::initialize();
        auto program = vk::easy::Context::get().createProgram();
        auto buffer = program->createResource<vk::easy::Buffer>();

        auto compute = program->createNode<vk::easy::ComputeNode>();
        auto stage = compute->getShaderStage();
        SpecializationData specializationData;
        stage->setShaderFile("headless.comp.spv")
            .defineConstant(0, offsetof(SpecializationData, BUFFER_ELEMENT_COUNT),
                sizeof(SpecializationData::BUFFER_ELEMENT_COUNT));
        compute->onUpdate([&specializationData](auto& node) {
            node.getShaderStage()->setConstantData(&specializationData, sizeof(SpecializationData));
            node.setDispatchSize(BUFFER_ELEMENTS, 1, 1);
        });

        std::vector<uint32_t> computeInput(BUFFER_ELEMENTS);
        std::vector<uint32_t> computeOutput(BUFFER_ELEMENTS);
        uint32_t n = 0;
        std::generate(computeInput.begin(), computeInput.end(), [&n] { return n++; });

        auto resourceWriter = program->createNode<vk::easy::ResourceWriteNode>();
        resourceWriter->onUpdate([&computeInput](auto& node) { node.setData(computeInput); });

        auto resourceReader = program->createNode<vk::easy::ResourceReadNode>();
        resourceReader->onUpdate([](auto& node) { node.setDataToRead(0); });
        resourceReader->onDataReady([&computeOutput](const auto& data) { computeOutput = data; });

        program->record();
        resourceWriter->writesTo(*buffer);
        compute->readsFrom(*buffer, 0);
        compute->writesTo(*buffer, 0);
        resourceReader->readsFrom(*buffer);
        program->run();

        std::cout << "Compute input:" << std::endl;
        for (auto& v : computeInput) {
            std::cout << v;
        }
        std::cout << std::endl;

        std::cout << "Compute output:" << std::endl;
        for (auto& v : computeOutput) {
            std::cout << v;
        }
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
