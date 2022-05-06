#include <vkEasy/resources/ColorAttachment.h>

using namespace VK_EASY_NAMESPACE;

ColorAttachment::ColorAttachment()
    : Image()
{
    m_descriptorType = vk::DescriptorType::eInputAttachment;
    setMemoryUsage(VMA_MEMORY_USAGE_GPU_ONLY);
    setDimensionality(vk::ImageType::e2D);
    m_imageAspect = vk::ImageAspectFlagBits::eColor;
    m_clearValue.setFloat32({ 0.0f, 0.0f, 0.0f, 1.0f });
}

vk::ImageLayout ColorAttachment::getRequiredLayout(vk::PipelineStageFlagBits stage, Access access)
{
    if (stage == vk::PipelineStageFlagBits::eNone)
        return vk::ImageLayout::ePresentSrcKHR;
    return vk::ImageLayout::eColorAttachmentOptimal;
}

void ColorAttachment::setClearColor(vk::ClearColorValue color)
{
    m_clearValue = color;
}

vk::ClearValue ColorAttachment::getClearValue()
{
    return m_clearValue;
}
