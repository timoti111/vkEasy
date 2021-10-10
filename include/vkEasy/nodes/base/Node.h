#pragma once
#include <functional>
#include <initializer_list>
#include <set>

#include <vkEasy/Error.h>
#include <vkEasy/global.h>
#include <vkEasy/resources/base/Resource.h>

namespace VK_EASY_NAMESPACE {
class Graph;

class Node : public Errorable {
    friend class Graph;

public:
    Node(Node const&) = delete;
    void operator=(Node const&) = delete;
    virtual ~Node() = default;

    void needsExtensions(const vk::ArrayProxyNoTemporaries<std::string>& extensions);
    void readsFrom(Resource* resource, uint32_t binding);
    void writesTo(Resource* resource, uint32_t binding);

protected:
    Node(const std::string& nodeName);
    void addToGraph();
    std::function<void()> m_updateFunction;

private:
    void execute();
    void setParent(Graph* parent);

    std::set<std::string> m_neededExtensions;
    Graph* m_parent;
};
} // namespace VK_EASY_NAMESPACE
