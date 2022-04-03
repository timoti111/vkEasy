#include <vkEasy/Device.h>
#include <vkEasy/resources/base/Image.h>

using namespace VK_EASY_NAMESPACE;

Image::Image()
    : Resource()
{
    m_imageCreateInfo.setSharingMode(vk::SharingMode::eExclusive);
}

void Image::addImageUsageFlag(vk::ImageUsageFlagBits flag)
{
    m_recreateResource = true;
    m_imageCreateInfo.setUsage(m_imageCreateInfo.usage | flag);
}

VkImage Image::getVkImage()
{
    return **dynamic_cast<MemoryAllocator::Image*>(m_vmaResource.get());
}

void Image::create()
{
    m_vmaResource = getDevice()->getAllocator()->createImage(m_imageCreateInfo, m_allocInfo);
    vk::ImageViewCreateInfo viewCreateInfo;
    vk::ComponentMapping components;
    components.setR(vk::ComponentSwizzle::eIdentity)
        .setG(vk::ComponentSwizzle::eIdentity)
        .setB(vk::ComponentSwizzle::eIdentity)
        .setA(vk::ComponentSwizzle::eIdentity);
    vk::ImageSubresourceRange subresourceRange;
    subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor)
        .setBaseMipLevel(0)
        .setLevelCount(1)
        .setBaseArrayLayer(0)
        .setLayerCount(1);
    viewCreateInfo.setImage(getVkImage())
        .setFormat(getFormat())
        .setViewType(vk::ImageViewType::e2D)
        .setComponents(components)
        .setSubresourceRange(subresourceRange);
    m_view = std::make_unique<vk::raii::ImageView>(*getDevice()->getLogicalDevice(), viewCreateInfo);
}

void Image::setDimensionality(const vk::ImageType& dimensionality)
{
    m_recreateResource = true;
    m_imageCreateInfo.setImageType(dimensionality);
}

vk::ImageType Image::getDimensionality()
{
    return m_imageCreateInfo.imageType;
}

void Image::setDimensions(const vk::Extent3D& dimensions)
{
    m_recreateResource = true;
    m_imageCreateInfo.setExtent(dimensions);
}

vk::Extent3D Image::getDimensions()
{
    return m_imageCreateInfo.extent;
}

void Image::setFormat(const vk::Format& format)
{
    m_recreateResource = true;
    m_imageCreateInfo.setFormat(format);
}

vk::Format Image::getFormat()
{
    return m_imageCreateInfo.format;
}

void Image::setMipLevels(uint32_t mipLevels)
{
    m_recreateResource = true;
    m_imageCreateInfo.setMipLevels(mipLevels);
}

uint32_t Image::getMipLevels()
{
    return m_imageCreateInfo.mipLevels;
}

void Image::setArrayLayers(uint32_t arrayLayers)
{
    m_recreateResource = true;
    m_imageCreateInfo.setArrayLayers(arrayLayers);
}

uint32_t Image::getArrayLayers()
{
    return m_imageCreateInfo.arrayLayers;
}

void Image::setSamplesPerTexel(const vk::SampleCountFlagBits& samplesPerTexel)
{
    m_recreateResource = true;
    m_imageCreateInfo.setSamples(samplesPerTexel);
}

vk::SampleCountFlagBits Image::getSamplesPerTexel()
{
    return m_imageCreateInfo.samples;
}

void Image::setImageTiling(const vk::ImageTiling& imageTiling)
{
    m_recreateResource = true;
    m_imageCreateInfo.setTiling(imageTiling);
}

vk::ImageTiling Image::getImageTiling()
{
    return m_imageCreateInfo.tiling;
}

vk::raii::ImageView* Image::getVkImageView(uint32_t imageIndex)
{
    return m_view.get();
}
