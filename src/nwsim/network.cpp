#include "network.hpp"
#include <memory>
#include <vector>
#include <queue>
#include <map>
#include <iostream>
#include <algorithm>
#include <limits.h>

using namespace NWSim;
std::shared_ptr<EndHost> Network::CreateEndHost(const std::string &address, float posX, float posY)
{
    std::cout << "Attempting to create EndHost: " << address << " ... ";
    std::shared_ptr<EndHost> n = nullptr;
    // Check if address is already used?
    if (!FindNode(address))
    {
        n = std::make_shared<EndHost>(0.0, 0.0, address);
        _nodes.push_back(n);
        std::cout << "Success!" << std::endl;
    }
    else
    {
        std::cout << "Fail!" << std::endl;
    }
    return n;
}

std::shared_ptr<Router> Network::CreateRouter(const std::string &address, float posX, float posY)
{
    std::cout << "Attempting to create Router: " << address << " ... ";
    std::shared_ptr<Router> n = nullptr;
    try
    {
        /* code */
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    // Check if address is already used?
    if (!FindNode(address))
    {
        n = std::make_shared<Router>(0.0, 0.0, address);
        _nodes.push_back(n);
        std::cout << "Success!" << std::endl;
    }
    else
    {
        std::cout << "Fail!" << std::endl;
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
    while (it_link != _links.end())
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
                // it_link now holds the proper reference, delete and jump up one
                it_link = _links.erase(it_link);
                continue;
            }
            if (n2 == node)
            {
                n1->DisconnectFromNode(node);
                // it_link now holds the proper reference, delete and jump up one
                it_link = _links.erase(it_link);
                continue;
            }
        }
        else
        {
            throw std::logic_error("Error: null Node pointers in Network::_links.");
        }
        it_link++; // since nothing was found, advance up
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
    if (n1 == nullptr || n2 == nullptr || n1 == n2)
    {
        throw std::logic_error("Error: Attempting to link invalid Nodes.");
    }
    // Check if already connected to target node. Since connections are bi-directional, only n1 needs to be checked
    if (n1->IsConnectedTo(n2))
    {
        throw std::logic_error("Error: Attempting to link already linked Nodes.");
    }
    std::shared_ptr<Link> link = std::make_shared<Link>();
    link->InitTransmissionQueues(n1, n2);
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
            auto l = std::get<2>(*it);
            l->RemoveNodeReferences();
            _links.erase(it);
        }
    }
}

void Network::GenerateRoutingTable()
{
    struct sortableNode {
        int cost;
        std::shared_ptr<Node> node;

    };
    struct smallerSortableNode {
        bool operator() (const sortableNode& n1, const sortableNode& n2)
        {
            return n1.cost < n2.cost;
        }
    };
    std::cout << "Generating routing table..." << std::endl;
    for (auto source : _nodes)
    {

        std::cout << "Handling source: " << source->network_interface.GetAddressStr();

        std::map<std::shared_ptr<Node>, int> dist;
        std::map<std::shared_ptr<Node>, bool> visited;
        std::map<std::shared_ptr<Node>, std::shared_ptr<Node>> comes_from;
        for (auto target : _nodes)
        {
            // skip self to self
            if (source != target)
            {
                dist[target] = INT_MAX;
                // TODO: Should initially fill _routingtable with nullptr? If so, should self-self be set to nullptr too?
            }
            _routingTable[{source->network_interface.GetAddressStr(),target->network_interface.GetAddressStr()}] = nullptr;
             
        }
        dist[source] = 0;
        std::cout << " - has " << dist.size() << " possible targets." << std::endl;
        
        std::priority_queue<sortableNode, std::vector<sortableNode>, smallerSortableNode> nxt;
        nxt.push({0,source});
        std::cout << "Starting nxt loop..." << std::endl;
        while (!nxt.empty())
        {
            auto current = nxt.top().node;
            nxt.pop();
            std::cout << "\tCurrent: " << current->network_interface.GetAddressStr() << std::endl;
            if (visited[current]) continue;
            visited[current] = true;

            std::vector<std::pair<std::shared_ptr<Link>, std::shared_ptr<Node>>> next_nodes;
            
            for (auto tmp : current->_connected) // checking current nodes connected nodes over networks _links, faster and avoid "criss cross" issue
            {
                next_nodes.push_back(std::make_pair(tmp.first.lock(), tmp.second.lock()));
            }
            for (auto candidate : next_nodes)
            {
                auto candidate_newdist = dist[current] + (int)candidate.first->GetTransmitCost();
                auto candidate_node = candidate.second;
                std::cout << "\t\tneighbor: " << candidate_node->network_interface.GetAddressStr() << 
                    "\twith newcost: " << candidate_newdist << 
                    "\t and oldcost: " << dist[candidate_node] << std::endl;
                if (candidate_newdist < dist[candidate_node])
                {
                    dist[candidate_node] = candidate_newdist;
                    comes_from[candidate_node] = current;
                    nxt.push({ -dist[candidate_node], candidate_node});
                }
            }
            // Set routing table
            for (auto x : comes_from)
            {
                _routingTable[std::make_pair(x.first->network_interface.GetAddressStr(),source->network_interface.GetAddressStr())] = x.second;
            }
        }
    }
    // for (auto source : _nodes) {

    // 	std::map<std::shared_ptr<Node>, int> dist;
    // 	std::map<std::shared_ptr<Node>, bool> visited;
    // 	std::map<std::shared_ptr<Node>, std::shared_ptr<Node>> comes_from;

    // 	for (auto target : _nodes) dist[target] = INT_MAX;
    // 	dist[source] = 0;

    // 	std::priority_queue<int, std::shared_ptr<Node>,std::less<int>> nxt;

    // 	nxt.push({ 0, source });

    // 	while (!nxt.empty()) {
    // 		std::shared_ptr<Node> a = nxt.top().second;
    // 		nxt.pop();
    // 		if (visited[a]) continue;
    // 		visited[a] = true;

    // 		std::vector<std::pair<std::shared_ptr<Node>, std::shared_ptr<Link>>> next_nodes;

    // 		for (auto t : _links) {
    // 			if (std::get<0>(t)) {
    // 				next_nodes.push_back({ std::get<1>(t), std::get<2>(t) });
    // 			}
    // 		}

    // 		for (auto p : next_nodes) {
    // 			if (dist[a] + (int)p.second->GetTransmitCost() < dist[p.first]) {
    // 				dist[p.first] = dist[a] + (int)p.second->GetTransmitCost();
    // 				comes_from[p.first] = a;
    // 				q.push({ -dist[p.first] , p.first });
    // 			}
    // 		}

    // 		// Set the routingtable

    // 		for (auto x : comes_from) {
    // 			_routingTable[{x.first->network_interface.GetAddressStr(), a->network_interface.GetAddressStr()}] = x.second;
    // 		}
    // 	}
    // }
}

void Network::PrintRoutingTable()
{
    std::cout << "Current routing table size: " << _routingTable.size() << std::endl;
    for (auto route : _routingTable)
    {
        auto current = route.first.first;
        auto target = route.first.second;
        auto sendto = (route.second == nullptr) ? "nullptr" : route.second->network_interface.GetAddressStr();
        std::cout << "\tWhen at: " << current << "\tand target: " << target << "\tsend to: " << sendto << std::endl;
    }
}