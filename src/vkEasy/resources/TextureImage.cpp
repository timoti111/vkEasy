#include <vkEasy/resources/TextureImage.h>

using namespace VK_EASY_NAMESPACE;

TextureImage::TextureImage()
    : ColorAttachment()
{
    m_descriptorType = vk::DescriptorType::eCombinedImageSampler;
    setMemoryUsage(VMA_MEMORY_USAGE_GPU_ONLY);
    addImageUsageFlag(vk::ImageUsageFlagBits::eSampled);
}

vk::ImageLayout TextureImage::getRequiredLayout(vk::PipelineStageFlagBits stage, Access access)
{
    if (stage == vk::PipelineStageFlagBits::eNone)
        return vk::ImageLayout::ePresentSrcKHR;
    return vk::ImageLayout::eShaderReadOnlyOptimal;
}