#pragma once
#include <functional>
#include <initializer_list>
#include <set>

#include <vkEasy/Error.h>
#include <vkEasy/global.h>
#include <vkEasy/resources/base/Resource.h>

namespace VK_EASY_NAMESPACE {
class Graph;
class Device;

class Node : public Errorable {
    friend class Graph;
    friend class Device;

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
    Graph* getParent();
    std::function<void(Device*)> m_updateFunction;

private:
    void execute(Device* device);
    void setParent(Graph* parent);

    std::set<std::string> m_neededExtensions;
    Graph* m_parent;
};
} // namespace VK_EASY_NAMESPACE
