#include <vkEasy/resources/DepthStencilBuffer.h>

using namespace VK_EASY_NAMESPACE;

DepthStencilBuffer::DepthStencilBuffer()
    : Image()
{
    m_descriptorType = vk::DescriptorType::eInputAttachment;
    setMemoryUsage(VMA_MEMORY_USAGE_GPU_ONLY);
    setFormat(vk::Format::eD32Sfloat);
    setDimensionality(vk::ImageType::e2D);
    m_imageAspect = vk::ImageAspectFlagBits::eDepth;
    m_clearValue.setDepth(1.0).setStencil(0);
}

vk::ImageLayout DepthStencilBuffer::getRequiredLayout(vk::PipelineStageFlagBits stage, Access access)
{
    if (stage == vk::PipelineStageFlagBits::eNone)
        return vk::ImageLayout::ePresentSrcKHR;
    return vk::ImageLayout::eDepthStencilAttachmentOptimal;
}
void DepthStencilBuffer::setClearValue(vk::ClearDepthStencilValue value)
{
    m_clearValue = value;
}

vk::ClearValue DepthStencilBuffer::getClearValue()
{
    return m_clearValue;
}
