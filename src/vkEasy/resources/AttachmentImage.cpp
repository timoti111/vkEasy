#include <vkEasy/resources/AttachmentImage.h>

using namespace VK_EASY_NAMESPACE;

AttachmentImage::AttachmentImage()
    : Image()
{
    m_descriptorType = vk::DescriptorType::eInputAttachment;
    setMemoryUsage(VMA_MEMORY_USAGE_GPU_ONLY);
}
void AttachmentImage::setIndex(size_t frameBufferIndex)
{
    m_index = frameBufferIndex;
}

size_t AttachmentImage::getIndex()
{
    return m_index;
}
