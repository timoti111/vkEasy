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

    void needsExtensions(const std::initializer_list<std::string>& extensions);
    void operator()();

protected:
    Node(const std::string& nodeName);
    void uses(Resource* resource);
    Graph* getGraph();
    std::function<void(Device*)> m_updateFunction;
    Node* m_nextNode;
    std::vector<Node*> m_dependantNodes;

    vk::QueueFlagBits m_queueType;

private:
    void execute();
    void setGraph(Graph* graph);
    void setNext(Node* next);

    std::set<Resource*> m_usedResources;
    std::set<std::string> m_neededExtensions;
    Graph* m_graph = nullptr;
    Device* m_device = nullptr;
};
} // namespace VK_EASY_NAMESPACE
