#pragma once
#include <vkEasy/global.h>
#include <vkEasy/resources/base/Image.h>

namespace VK_EASY_NAMESPACE {
class Attachment : public Image {

public:
    Attachment(Attachment const&) = delete;
    void operator=(Attachment const&) = delete;
    virtual ~Attachment() = default;

protected:
    Attachment();
};
}
