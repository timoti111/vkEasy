#include <vkEasy/Graph.h>
#include <vkEasy/WSI.h>

using namespace VK_EASY_NAMESPACE;

WSI::WSI(uint32_t width, uint32_t height, const std::string& title, Graph* parent)
    : Errorable("WSI")
    , m_name(title)
    , m_extent(width, height)
{
    m_swapChain = &parent->createResource<SwapChainImage>();
    m_swapChain->m_window = this;
}

void WSI::update()
{
    if (!m_surface)
        createSurface();
}

AttachmentImage* WSI::getAttachment()
{
    return m_swapChain;
}