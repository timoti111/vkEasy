#pragma once
#include <vkEasy/MemoryAllocator.h>
#include <vkEasy/global.h>
#include <vkEasy/resources/base/Resource.h>

namespace VK_EASY_NAMESPACE {
class Image : public Resource {
public:
    Image(Image const&) = delete;
    void operator=(Image const&) = delete;
    virtual ~Image() = default;

    void addImageUsageFlag(vk::ImageUsageFlagBits flag);
    VkImage getVkImage();
    virtual vk::raii::ImageView* getVkImageView(uint32_t imageIndex);

    void setDimensionality(const vk::ImageType& dimensionality);
    vk::ImageType getDimensionality();

    void setDimensions(const vk::Extent3D& dimensions);
    vk::Extent3D getDimensions();

    void setFormat(const vk::Format& format);
    vk::Format getFormat();

    void setMipLevels(uint32_t mipLevels);
    uint32_t getMipLevels();

    void setArrayLayers(uint32_t arrayLayers);
    uint32_t getArrayLayers();

    void setSamplesPerTexel(const vk::SampleCountFlagBits& samplesPerTexel);
    vk::SampleCountFlagBits getSamplesPerTexel();

    void setImageTiling(const vk::ImageTiling& imageTiling);
    vk::ImageTiling getImageTiling();

protected:
    Image();
    virtual void create();

    vk::ImageUsageFlags m_imageUsageFlags;
    vk::ImageCreateInfo m_imageCreateInfo;
    std::unique_ptr<vk::raii::ImageView> m_view;
};
}
