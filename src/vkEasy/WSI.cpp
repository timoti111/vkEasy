#include <vkEasy/WSI.h>

using namespace VK_EASY_NAMESPACE;

WSI::WSI(uint32_t width, uint32_t height, const std::string& title, Device* parent)
    : Errorable("WSI")
    , m_name(title)
    , m_extent(width, height)
    , m_parent(parent)
    , m_swapChain(this)
{
}

void WSI::update()
{
    if (!m_surface)
        createSurface();
}
