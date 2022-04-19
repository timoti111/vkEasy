#include <vkEasy/resources/AttachmentImage.h>

using namespace VK_EASY_NAMESPACE;

AttachmentImage::AttachmentImage()
    : Image()
{
    m_descriptorType = vk::DescriptorType::eInputAttachment;
    setMemoryUsage(VMA_MEMORY_USAGE_GPU_ONLY);
    m_imageAspect = vk::ImageAspectFlagBits::eColor;
}
void AttachmentImage::setIndex(size_t frameBufferIndex)
{
    m_index = frameBufferIndex;
}

size_t AttachmentImage::getIndex()
{
    return m_index;
}

vk::ImageLayout AttachmentImage::getRequiredLayout(vk::PipelineStageFlagBits stage, Access access)
{
    if (stage == vk::PipelineStageFlagBits::eNone)
        return vk::ImageLayout::ePresentSrcKHR;
    return vk::ImageLayout::eColorAttachmentOptimal;
}
