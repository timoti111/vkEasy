#include <iostream>
#include <vkEasy/Device.h>
#include <vkEasy/resources/base/Image.h>

using namespace VK_EASY_NAMESPACE;

Image::Image()
    : Resource()
{
    setImageTiling(vk::ImageTiling::eOptimal);
    addImageUsageFlag(vk::ImageUsageFlagBits::eTransferSrc);
    addImageUsageFlag(vk::ImageUsageFlagBits::eTransferDst);
    m_imageCreateInfo.setInitialLayout(vk::ImageLayout::eUndefined);
}

void Image::addImageUsageFlag(vk::ImageUsageFlagBits flag)
{
    m_imageCreateInfo.setUsage(m_imageCreateInfo.usage | flag);
}

VkImage Image::getVkImage()
{
    return m_images[getActualFrameIndex()];
}

void Image::create()
{
    getLastImageAccessInfo().lastLayout = vk::ImageLayout::eUndefined;

    if (m_queueIndicesVector.size() > 1) {
        m_imageCreateInfo.setQueueFamilyIndices(m_queueIndicesVector);
        m_imageCreateInfo.setSharingMode(vk::SharingMode::eConcurrent);
    }
    m_vmaResource[getActualFrameIndex()] = getDevice()->getAllocator()->createImage(m_imageCreateInfo, m_allocInfo);
    m_images[getActualFrameIndex()]
        = **dynamic_cast<MemoryAllocator::Image*>(m_vmaResource[getActualFrameIndex()].get());
}

void Image::setDimensionality(const vk::ImageType& dimensionality)
{
    m_imageCreateInfo.setImageType(dimensionality);
}

vk::ImageType Image::getDimensionality()
{
    return m_imageCreateInfo.imageType;
}

void Image::setDimensions(const vk::Extent3D& dimensions)
{
    m_imageCreateInfo.setExtent(dimensions);
}

vk::Extent3D Image::getDimensions()
{
    return m_imageCreateInfo.extent;
}

void Image::setFormat(const vk::Format& format)
{
    m_imageCreateInfo.setFormat(format);
}

vk::Format Image::getFormat()
{
    return m_imageCreateInfo.format;
}

void Image::setMipLevels(uint32_t mipLevels)
{
    m_imageCreateInfo.setMipLevels(mipLevels);
}

uint32_t Image::getMipLevels()
{
    return m_imageCreateInfo.mipLevels;
}

void Image::setArrayLayers(uint32_t arrayLayers)
{
    m_imageCreateInfo.setArrayLayers(arrayLayers);
}

uint32_t Image::getArrayLayers()
{
    return m_imageCreateInfo.arrayLayers;
}

void Image::setSamplesPerTexel(const vk::SampleCountFlagBits& samplesPerTexel)
{
    m_imageCreateInfo.setSamples(samplesPerTexel);
}

vk::SampleCountFlagBits Image::getSamplesPerTexel()
{
    return m_imageCreateInfo.samples;
}

void Image::setImageTiling(const vk::ImageTiling& imageTiling)
{
    m_imageCreateInfo.setTiling(imageTiling);
}

vk::ImageTiling Image::getImageTiling()
{
    return m_imageCreateInfo.tiling;
}

vk::raii::ImageView* Image::getVkImageView(uint32_t imageIndex)
{
    return m_views[imageIndex].get();
}

void Image::createView(size_t index)
{
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
    viewCreateInfo.setFormat(getFormat())
        .setViewType(vk::ImageViewType::e2D)
        .setComponents(components)
        .setSubresourceRange(subresourceRange)
        .setImage(m_images[index]);
    m_views[index] = std::make_unique<vk::raii::ImageView>(*getDevice()->getLogicalDevice(), viewCreateInfo);
}

void Image::solveSynchronization(vk::PipelineStageFlagBits stage, Access access)
{
    auto& lastAccess = getLastAccessInfo();
    auto& lastImageAccess = getLastImageAccessInfo();
    auto nextStage = stage;
    auto nextAccess = access == Access::ReadOnly ? vk::AccessFlagBits::eMemoryRead : vk::AccessFlagBits::eMemoryWrite;
    auto nextLayout = getRequiredLayout(nextStage, access);
    bool layoutChanged = false;

    vk::ImageMemoryBarrier imageBarrier;
    vk::ImageSubresourceRange range;
    range.setBaseMipLevel(0)
        .setLevelCount(VK_REMAINING_MIP_LEVELS)
        .setBaseArrayLayer(0)
        .setLayerCount(VK_REMAINING_ARRAY_LAYERS)
        .setAspectMask(m_imageAspect);
    imageBarrier.setImage(getVkImage())
        .setSubresourceRange(range)
        .setSrcAccessMask(lastAccess.lastAccess ? lastAccess.lastAccess.value() : vk::AccessFlagBits::eNone)
        .setDstAccessMask(nextAccess)
        .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
        .setOldLayout(lastImageAccess.lastLayout ? lastImageAccess.lastLayout.value() : vk::ImageLayout::eUndefined)
        .setNewLayout(nextLayout);

    if (lastAccess.lastRead && access == Access::ReadWrite) {
        insertImageBarrier(lastAccess.lastRead.value(), nextStage, imageBarrier);
        layoutChanged = true;
    }
    if (lastAccess.lastWrite && access == Access::ReadWrite) {
        insertImageBarrier(lastAccess.lastWrite.value(), nextStage, imageBarrier);
        layoutChanged = true;
    }
    if (imageBarrier.oldLayout != imageBarrier.newLayout && !layoutChanged)
        insertImageBarrier(
            vk::PipelineStageFlagBits::eAllCommands, vk::PipelineStageFlagBits::eAllCommands, imageBarrier);

    if (access == Access::ReadOnly)
        lastAccess.lastRead = nextStage;
    if (access == Access::ReadWrite)
        lastAccess.lastWrite = nextStage;
    lastAccess.lastAccess = nextAccess;
    lastImageAccess.lastLayout = nextLayout;
}

Image::ImageAccessInfo& Image::getLastImageAccessInfo()
{
    return m_lastImageAccess[getActualFrameIndex()];
}

void Image::insertImageBarrier(
    vk::PipelineStageFlagBits src, vk::PipelineStageFlagBits dst, const vk::ImageMemoryBarrier& barrier)
{
    auto buffers = getGraph()->getCommandBuffers(1);
    if (buffers.empty())
        return;
#ifndef NDEBUG
    std ::cout << "Adding image barrier between { " << vk::to_string(src) << " } and { " << vk::to_string(dst) << " }"
               << std::endl;
#endif
    buffers[0]->pipelineBarrier(src, dst, {}, {}, {}, barrier);
}
