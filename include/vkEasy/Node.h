#pragma once
#include <functional>
#include <initializer_list>
#include <set>

#include <vkEasy/Resource.h>
#include <vkEasy/global.h>

namespace VK_EASY_NAMESPACE {
class Node {
public:
    virtual ~Node() = default;

    void skipIf(std::function<bool()> predicate);
    void needsExtensions(const vk::ArrayProxyNoTemporaries<Resource>& extensions);

protected:
    Node();
    std::function<void()> m_updateFunction;

private:
    void prepare();
    void execute();

    std::function<void()> m_skipIfFunction;
    std::set<std::string> m_neededExtensions;
};
} // namespace VK_EASY_NAMESPACE
