#include <fstream>
#include <iostream>
#include <vector>
#include <vulkan/vulkan_raii.hpp>

#define DEBUG (!NDEBUG)
#define BUFFER_ELEMENTS 32
#define LOG(...) printf(__VA_ARGS__)

class VulkanExample {
public:
    VulkanExample();

private:
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes,
        VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData, void*);
    void createBuffer(vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags,
        std::unique_ptr<vk::raii::Buffer>& buffer, std::unique_ptr<vk::raii::DeviceMemory>& memory, vk::DeviceSize size,
        void* data = nullptr);
    std::unique_ptr<vk::raii::ShaderModule> loadShader(const std::string& fileName);

    uint32_t queueFamilyIndex;

    // Nondestroyables
    std::unique_ptr<vk::raii::Queue> queue;
    std::unique_ptr<vk::raii::PhysicalDevices> physicalDevices;
    vk::raii::PhysicalDevice* physicalDevice;

    // Destroyables - must be destroyed in opposite order as seen here
    std::unique_ptr<vk::raii::Context> context;
    std::unique_ptr<vk::raii::Instance> instance;
    std::unique_ptr<vk::raii::DebugUtilsMessengerEXT> debugUtilsMessenger;
    std::unique_ptr<vk::raii::Device> device;
    std::unique_ptr<vk::raii::ShaderModule> shaderModule;
    std::unique_ptr<vk::raii::CommandPool> commandPool;
    std::unique_ptr<vk::raii::CommandBuffers> commandBuffers;
    vk::raii::CommandBuffer* commandBuffer;
    std::unique_ptr<vk::raii::Fence> fence;
    std::unique_ptr<vk::raii::PipelineCache> pipelineCache;
    std::unique_ptr<vk::raii::Pipeline> pipeline;
    std::unique_ptr<vk::raii::DescriptorPool> descriptorPool;
    std::unique_ptr<vk::raii::DescriptorSetLayout> descriptorSetLayout;
    std::unique_ptr<vk::raii::DescriptorSets> descriptorSets;
    vk::raii::DescriptorSet* descriptorSet;
    std::unique_ptr<vk::raii::PipelineLayout> pipelineLayout;
};

int main()
{
    try {
        auto vulkanExample = std::make_unique<VulkanExample>();
    } catch (vk::SystemError& err) {
        std::cout << "vk::SystemError: " << err.what() << std::endl;
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

VulkanExample::VulkanExample()
{
    LOG("Running headless compute example\n");

    context = std::make_unique<vk::raii::Context>();

    vk::ApplicationInfo appInfo;
    appInfo.setPApplicationName("Vulkan headless example")
        .setPEngineName("VulkanExample")
        .setApiVersion(VK_API_VERSION_1_0);

    /*
                Vulkan instance creation (without surface extensions)
        */
    vk::InstanceCreateInfo instanceCreateInfo;
    instanceCreateInfo.setPApplicationInfo(&appInfo);

    std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
#ifndef NDEBUG
    std::vector<vk::LayerProperties> layerProperties = context->enumerateInstanceLayerProperties();

    bool layersAvailable = true;
    for (auto layerName : validationLayers) {
        bool layerAvailable = false;
        for (auto instanceLayer : layerProperties) {
            if ((strcmp(layerName, instanceLayer.layerName) == 0)) {
                layerAvailable = true;
                break;
            }
        }
        if (!layerAvailable) {
            layersAvailable = false;
            break;
        }
    }

    std::vector<const char*> validationExt = { VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
    if (layersAvailable) {
        instanceCreateInfo.setPEnabledLayerNames(validationLayers);
        instanceCreateInfo.setPEnabledExtensionNames(validationExt);
    }

    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning);
    vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
        | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

    vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo;
    debugMessengerCreateInfo.setMessageSeverity(severityFlags)
        .setMessageType(messageTypeFlags)
        .setPfnUserCallback(debugUtilsMessengerCallback);
    instanceCreateInfo.setPNext(&debugMessengerCreateInfo);
#endif

    instance = std::make_unique<vk::raii::Instance>(*context, instanceCreateInfo);

#if DEBUG
    if (layersAvailable)
        debugUtilsMessenger = std::make_unique<vk::raii::DebugUtilsMessengerEXT>(*instance, debugMessengerCreateInfo);
#endif

    /*
                Vulkan device creation
        */
    // Physical device (always use first)
    physicalDevices = std::make_unique<vk::raii::PhysicalDevices>(*instance);
    physicalDevice = &physicalDevices->at(0);
    auto deviceProperties = physicalDevice->getProperties();
    LOG("GPU: %s\n", deviceProperties.deviceName.data());

    // Request a single compute queue
    const float defaultQueuePriority(0.0f);
    vk::DeviceQueueCreateInfo queueCreateInfo;
    auto queueFamilyProperties = physicalDevice->getQueueFamilyProperties();

    int queueIndex = 0;
    for (auto& queueFamilyProperty : queueFamilyProperties) {
        if (queueFamilyProperty.queueFlags & vk::QueueFlagBits::eCompute) {
            queueFamilyIndex = queueIndex;
            queueCreateInfo.setQueueFamilyIndex(queueIndex).setQueueCount(1).setPQueuePriorities(&defaultQueuePriority);
            break;
        }
        queueIndex++;
    }

    // Create logical device
    vk::DeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.setQueueCreateInfos(queueCreateInfo);
    device = std::make_unique<vk::raii::Device>(*physicalDevice, deviceCreateInfo);

    // Get a compute queue
    queue = std::make_unique<vk::raii::Queue>(*device, queueFamilyIndex, 0);

    // Compute command pool
    vk::CommandPoolCreateInfo cmdPoolInfo;
    cmdPoolInfo.setQueueFamilyIndex(queueFamilyIndex).setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    commandPool = std::make_unique<vk::raii::CommandPool>(*device, cmdPoolInfo);

    /*
                Prepare storage buffers
        */
    std::vector<uint32_t> computeInput(BUFFER_ELEMENTS);
    std::vector<uint32_t> computeOutput(BUFFER_ELEMENTS);

    // Fill input data
    uint32_t n = 0;
    std::generate(computeInput.begin(), computeInput.end(), [&n] { return n++; });

    const vk::DeviceSize bufferSize = BUFFER_ELEMENTS * sizeof(uint32_t);

    std::unique_ptr<vk::raii::Buffer> deviceBuffer, hostBuffer;
    std::unique_ptr<vk::raii::DeviceMemory> deviceMemory, hostMemory;

    // Copy input data to VRAM using a staging buffer
    {
        createBuffer(vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst,
            vk::MemoryPropertyFlagBits::eHostVisible, hostBuffer, hostMemory, bufferSize, computeInput.data());

        // Flush writes to host visible buffer
        auto mapped = hostMemory->mapMemory(0, VK_WHOLE_SIZE);
        vk::MappedMemoryRange mappedRange;
        mappedRange.setMemory(**hostMemory).setOffset(0).setSize(VK_WHOLE_SIZE);
        device->flushMappedMemoryRanges(mappedRange);
        hostMemory->unmapMemory();

        createBuffer(vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferSrc
                | vk::BufferUsageFlagBits::eTransferDst,
            vk::MemoryPropertyFlagBits::eDeviceLocal, deviceBuffer, deviceMemory, bufferSize);

        // Create a command buffer for compute operations
        vk::CommandBufferAllocateInfo cmdBufAllocateInfo;
        cmdBufAllocateInfo.setCommandPool(**commandPool)
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(1);
        commandBuffers = std::make_unique<vk::raii::CommandBuffers>(*device, cmdBufAllocateInfo);
        commandBuffer = &commandBuffers->at(0);

        vk::CommandBufferBeginInfo cmdBufInfo;
        commandBuffer->begin(cmdBufInfo);

        commandBuffer->pipelineBarrier(
            vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, {});

        vk::BufferCopy copyRegion;
        copyRegion.setSize(bufferSize);
        commandBuffer->copyBuffer(**hostBuffer, **deviceBuffer, copyRegion);
    }

    /*
                Prepare compute pipeline
        */
    {
        vk::DescriptorPoolSize poolSize;
        poolSize.setType(vk::DescriptorType::eStorageBuffer).setDescriptorCount(1);

        vk::DescriptorPoolCreateInfo descriptorPoolInfo;
        descriptorPoolInfo.setPoolSizes(poolSize).setMaxSets(1).setFlags(
            vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
        descriptorPool = std::make_unique<vk::raii::DescriptorPool>(*device, descriptorPoolInfo);

        vk::DescriptorSetLayoutBinding setLayoutBinding;
        setLayoutBinding.setDescriptorType(vk::DescriptorType::eStorageBuffer)
            .setStageFlags(vk::ShaderStageFlagBits::eCompute)
            .setBinding(0)
            .setDescriptorCount(1);
        vk::DescriptorSetLayoutCreateInfo descriptorLayout;
        descriptorLayout.setBindings(setLayoutBinding);
        descriptorSetLayout = std::make_unique<vk::raii::DescriptorSetLayout>(*device, descriptorLayout);

        vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
        pipelineLayoutCreateInfo.setSetLayouts(**descriptorSetLayout);
        pipelineLayout = std::make_unique<vk::raii::PipelineLayout>(*device, pipelineLayoutCreateInfo);

        vk::DescriptorSetAllocateInfo allocInfo;
        allocInfo.setDescriptorPool(**descriptorPool).setSetLayouts(**descriptorSetLayout);
        descriptorSets = std::make_unique<vk::raii::DescriptorSets>(*device, allocInfo);
        descriptorSet = &descriptorSets->at(0);

        vk::DescriptorBufferInfo bufferDescriptor;
        bufferDescriptor.setBuffer(**deviceBuffer).setOffset(0).setRange(VK_WHOLE_SIZE);
        vk::WriteDescriptorSet computeWriteDescriptorSet;
        computeWriteDescriptorSet.setDstSet(**descriptorSet)
            .setDescriptorType(vk::DescriptorType::eStorageBuffer)
            .setDstBinding(0)
            .setBufferInfo(bufferDescriptor)
            .setDescriptorCount(1);
        device->updateDescriptorSets(computeWriteDescriptorSet, {});

        vk::PipelineCacheCreateInfo pipelineCacheCreateInfo;
        pipelineCache = std::make_unique<vk::raii::PipelineCache>(*device, pipelineCacheCreateInfo);

        // Create pipeline
        vk::ComputePipelineCreateInfo computePipelineCreateInfo;
        computePipelineCreateInfo.setLayout(**pipelineLayout);

        // Pass SSBO size via specialization constant
        struct SpecializationData {
            uint32_t BUFFER_ELEMENT_COUNT = BUFFER_ELEMENTS;
        } specializationData;
        vk::SpecializationMapEntry specializationMapEntry;
        specializationMapEntry.setConstantID(0).setOffset(0).setSize(sizeof(uint32_t));
        vk::SpecializationInfo specializationInfo;
        specializationInfo.setMapEntries(specializationMapEntry).setData<SpecializationData>(specializationData);

        shaderModule = loadShader("headless.comp.spv");

        vk::PipelineShaderStageCreateInfo shaderStage;
        shaderStage.setStage(vk::ShaderStageFlagBits::eCompute)
            .setModule(**shaderModule)
            .setPName("main")
            .setPSpecializationInfo(&specializationInfo);

        assert(shaderModule);
        computePipelineCreateInfo.stage = shaderStage;
        pipeline = std::make_unique<vk::raii::Pipeline>(*device, *pipelineCache, computePipelineCreateInfo);
    }

    /*
                Command buffer creation (for compute work submission)
        */
    {
        commandBuffer->pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eComputeShader, {}, {}, {}, {});

        commandBuffer->bindPipeline(vk::PipelineBindPoint::eCompute, **pipeline);
        commandBuffer->bindDescriptorSets(vk::PipelineBindPoint::eCompute, **pipelineLayout, 0, **descriptorSet, {});

        commandBuffer->dispatch(BUFFER_ELEMENTS, 1, 1);

        commandBuffer->pipelineBarrier(
            vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, {});

        // Read back to host visible buffer
        vk::BufferCopy copyRegion;
        copyRegion.setSize(bufferSize);
        commandBuffer->copyBuffer(**deviceBuffer, **hostBuffer, copyRegion);

        commandBuffer->pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eHost, {}, {}, {}, {});

        commandBuffer->end();

        // Fence for compute CB sync
        vk::FenceCreateInfo fenceCreateInfo;
        // fenceCreateInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
        fence = std::make_unique<vk::raii::Fence>(*device, fenceCreateInfo);

        // Submit compute work
        device->resetFences(**fence);
        // const vk::PipelineStageFlags waitStageMask = vk::PipelineStageFlagBits::eTransfer;
        vk::SubmitInfo computeSubmitInfo;
        computeSubmitInfo.setCommandBuffers(**commandBuffer);
        // computeSubmitInfo.setWaitDstStageMask(waitStageMask).setCommandBuffers(**commandBuffer).setWaitSemaphores({});
        queue->submit(computeSubmitInfo, **fence);
        auto result = device->waitForFences(**fence, VK_TRUE, UINT64_MAX);

        // Make device writes visible to the host
        auto mapped = hostMemory->mapMemory(0, VK_WHOLE_SIZE);
        vk::MappedMemoryRange mappedRange;
        mappedRange.setMemory(**hostMemory).setOffset(0).setSize(VK_WHOLE_SIZE);
        device->invalidateMappedMemoryRanges(mappedRange);

        // Copy to output
        memcpy(computeOutput.data(), mapped, bufferSize);
        hostMemory->unmapMemory();
    }

    queue->waitIdle();

    // Output buffer contents
    LOG("Compute input:\n");
    for (auto v : computeInput) {
        LOG("%d \t", v);
    }
    std::cout << std::endl;

    LOG("Compute output:\n");
    for (auto v : computeOutput) {
        LOG("%d \t", v);
    }
    std::cout << std::endl;
}

VkBool32 VulkanExample::debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
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

void VulkanExample::createBuffer(vk::BufferUsageFlags usageFlags, vk::MemoryPropertyFlags memoryPropertyFlags,
    std::unique_ptr<vk::raii::Buffer>& buffer, std::unique_ptr<vk::raii::DeviceMemory>& memory, vk::DeviceSize size,
    void* data)
{
    // Create the buffer handle
    vk::BufferCreateInfo bufferCreateInfo;
    bufferCreateInfo.setUsage(usageFlags).setSize(size).setSharingMode(vk::SharingMode::eExclusive);
    buffer = std::make_unique<vk::raii::Buffer>(*device, bufferCreateInfo);

    // Create the memory backing up the buffer handle
    auto deviceMemoryProperties = physicalDevices->at(0).getMemoryProperties();
    auto memReqs = buffer->getMemoryRequirements();
    vk::MemoryAllocateInfo memAlloc;
    memAlloc.setAllocationSize(memReqs.size);

    // Find a memory type index that fits the properties of the buffer
    bool memTypeFound = false;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memReqs.memoryTypeBits & 1) == 1) {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags) {
                memAlloc.memoryTypeIndex = i;
                memTypeFound = true;
            }
        }
        memReqs.memoryTypeBits >>= 1;
    }
    assert(memTypeFound);
    memory = std::make_unique<vk::raii::DeviceMemory>(*device, memAlloc);

    if (data != nullptr) {
        auto mapped = memory->mapMemory(0, size);
        memcpy(mapped, data, size);
        memory->unmapMemory();
    }
    buffer->bindMemory(**memory, 0);
}

std::unique_ptr<vk::raii::ShaderModule> VulkanExample::loadShader(const std::string& fileName)
{
    std::ifstream is(fileName, std::ios::binary | std::ios::in | std::ios::ate);

    if (is.is_open()) {
        size_t size = is.tellg();
        is.seekg(0, std::ios::beg);
        std::vector<char> shaderCode(size);
        is.read(shaderCode.data(), size);
        is.close();

        assert(size > 0);

        vk::ShaderModuleCreateInfo moduleCreateInfo;
        moduleCreateInfo.setCodeSize(size).setPCode((uint32_t*)shaderCode.data());
        return std::make_unique<vk::raii::ShaderModule>(*device, moduleCreateInfo);
    } else {
        std::cerr << "Error: Could not open shader file \"" << fileName << "\""
                  << "\n";
        return std::unique_ptr<vk::raii::ShaderModule>();
    }
}

// Barrier to ensure that input buffer transfer is finished before compute shader reads from it
// vk::BufferMemoryBarrier bufferBarrier;
// bufferBarrier.setBuffer(**deviceBuffer)
//     .setSize(VK_WHOLE_SIZE)
//     .setSrcAccessMask(vk::AccessFlagBits::eHostWrite)
//     .setDstAccessMask(vk::AccessFlagBits::eShaderRead)
//     .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
//     .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);

// commandBuffer->pipelineBarrier(
//     vk::PipelineStageFlagBits::eHost, vk::PipelineStageFlagBits::eComputeShader, {}, {}, bufferBarrier, {});

// Barrier to ensure that shader writes are finished before buffer is read back from GPU
// bufferBarrier.setBuffer(**deviceBuffer)
//     .setSize(VK_WHOLE_SIZE)
//     .setSrcAccessMask(vk::AccessFlagBits::eShaderWrite)
//     .setDstAccessMask(vk::AccessFlagBits::eTransferRead)
//     .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
//     .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);

// commandBuffer->pipelineBarrier(
//     vk::PipelineStageFlagBits::eComputeShader, vk::PipelineStageFlagBits::eTransfer, {}, {}, bufferBarrier,
//     {});

// Barrier to ensure that buffer copy is finished before host reading from it
// bufferBarrier.setBuffer(**hostBuffer)
//     .setSize(VK_WHOLE_SIZE)
//     .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
//     .setDstAccessMask(vk::AccessFlagBits::eHostRead)
//     .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
//     .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);

// commandBuffer->pipelineBarrier(
//     vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eHost, {}, {}, bufferBarrier, {});