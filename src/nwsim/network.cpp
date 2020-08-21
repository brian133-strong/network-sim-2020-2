#include "network.hpp"
#include <memory>
#include <vector>
#include <algorithm>
#include <limits.h>

using namespace NWSim;
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
    link->InitTransmissionQueues(n1,n2);
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


void Network::GenerateRoutingTable()
{
    // Collection of all target address / (intermediate) source node pairs
    /*std::vector<std::pair<std::string, std::string>> targetSourcePairs;
    for (auto source : _nodes)
    {
        auto sourceLinkedTo = source->_connected;

        for (auto target : _nodes)
        {
            // not inserting self to self
            if (target == source)
                continue;
            std::pair<std::string, std::string> targetSourcePair = std::make_pair(target->network_interface.GetAddressStr(), source->network_interface.GetAddressStr());

            std::shared_ptr<Node> route = nullptr;

            for (auto link : _links)
            {
                auto n1 = std::get<0>(link);
                auto n2 = std::get<1>(link);
                auto with = std::get<2>(link);

                //_routingTable.insert(std::make_pair(targetSourcePair,route));
            }
        }
    }*/

	// new 

	for (auto source : _nodes) {

		std::map<Node, int> dist;
		std::map<Node, bool> visited;
		std::map<Node, Node> comes_from;

		for (auto target : _nodes) dist[target] = 1e9;
		dist[source] = 0;

		std::priority_queue<Node, int> nxt;

		nxt.push({ 0, source });

		while (!nxt.empty()) {
			Node a = nxt.top().second; 
			nxt.pop();
			if (visited[a]) continue;
			visited[a] = true;

			std::vector<std::pair<std::shared_ptr<Node>, std::shared_ptr<Link>>> next_nodes;

			for (auto t : _links) {
				if (std::get<0>(t)) {
					next_nodes.push_back({ std::get<1>(t), std::get<2>(t) });
				}
			}

			for (auto p : next_nodes) {
				if (dist[a] + (int)p.second.GetTransmitCost() < dist[p.first]) {
					dist[p.first] = dist[a] + (int)p.second.GetTransmitCost();
					comes_from[p.first] = a;
					q.push({ -dist[p.first] , p.first });
				}
			}

			// Set the routingtable

			for (auto x : comes_from) {
				_routingTable[{x.first, a}] = x.second;
			}

		}

	}
}