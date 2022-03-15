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
    m_vmaResource = m_device->getAllocator()->createImage(m_imageCreateInfo, m_allocInfo);
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

void Image::setArrayLevels(uint32_t arrayLevels)
{
    m_recreateResource = true;
    m_imageCreateInfo.setArrayLayers(arrayLevels);
}

uint32_t Image::getArrayLevels()
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
