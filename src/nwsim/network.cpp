#include "network.hpp"
#include <memory>
#include <vector>
#include <algorithm>

std::shared_ptr<Node> Network::CreateNode(const std::string &address)
{
    std::cout << "Creating node: " << address << std::endl;
    std::shared_ptr<Node> n = nullptr;
    // Check if address is already used?
    if (!FindNode(address))
    {
        n = std::make_shared<Node>(0.0, 0.0, address);
        _nodes.push_back(n);
        std::cout << "pushed to _nodes" << std::endl;
    }
    return n;
}

std::shared_ptr<Node> Network::FindNode(const std::string &address) const
{
    std::shared_ptr<Node> node = nullptr;
    auto it_node = std::find_if(_nodes.begin(), _nodes.end(),
                                [address](std::shared_ptr<Node> n) {
                                    return n == address;
                                });
    if (it_node != _nodes.end())
    {
        node = (*it_node);
    }
    return node;
}

void Network::RemoveNode(std::shared_ptr<Node> node)
{
    // Need to delete all references to this node first.
    // Find if node is used for any links?
    auto it_link = _links.begin();
    for (; it_link != _links.end(); it_link++)
    {
        // grab the nodes as shared_ptr from tuple
        auto n1 = std::get<0>(*it_link);
        auto n2 = std::get<1>(*it_link);
        // check if null, they should never be null though..
        if (n1 != nullptr && n2 != nullptr)
        {
            // check if either of the link ends is equal to the node
            if (n1 == node)
            {
                n2->DisconnectFromNode(node);
                // it_link now holds the proper reference
                break;
            }
            if (n2 == node)
            {
                n1->DisconnectFromNode(node);
                // it_link now holds the proper reference
                break;
            }
        }
        else
        {
            throw std::logic_error("Error: null Node pointers in Network::_links.");
        }
    }
    // drop the tuple, removes link, shared_ptr nodes and also the links (possibly non-empty) packet queue which holds weak_ptr to Node
    if (it_link != _links.end())
    {
        _links.erase(it_link);
    }

    // Find and erase the node in _nodes.
    auto it_node = std::find_if(_nodes.begin(), _nodes.end(),
                                [node](const std::shared_ptr<Node> n) {
                                    return node == n;
                                });
    if ((*it_node) == node)
    {
        _nodes.erase(it_node);
    }
}

std::shared_ptr<Link> Network::LinkNodes(std::shared_ptr<Node> n1, std::shared_ptr<Node> n2)
{
    // Only work with valid nodes
    if (n1 == nullptr || n2 == nullptr)
    {
        throw std::logic_error("Error: Attempting to link invalid Nodes.");
    }
    // Check if already connected to target node. Since connections are bi-directional, only n1 needs to be checked
    if (n1->IsConnectedTo(n2))
    {
        throw std::logic_error("Error: Attempting to link already linked Nodes.");
    }
    std::shared_ptr<Link> link = std::make_shared<Link>();
    // References to this new link exist on both nodes and at the network
    n1->ConnectToNode(n2, link);
    n2->ConnectToNode(n1, link);
    _links.push_back(std::make_tuple(n1, n2, link));
    return link;
}

void Network::RemoveLink(std::shared_ptr<Node> n1, std::shared_ptr<Node> n2)
{
    // Only work with valid nodes
    if (n1 == nullptr || n2 == nullptr)
    {
        throw std::logic_error("Error: Attempting to link invalid Nodes.");
    }
    n1->DisconnectFromNode(n2);
    n2->DisconnectFromNode(n1);
    for (auto it = _links.begin(); it != _links.end(); it++)
    {
        if (n1 == std::get<0>(*it) && n2 == std::get<1>(*it) ||
            n2 == std::get<0>(*it) && n1 == std::get<1>(*it))
        {
            _links.erase(it);
            break;
        }
    }
}