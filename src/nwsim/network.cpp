#include "network.hpp"
#include <memory>
#include <vector>
#include <queue>
#include <map>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <limits.h>

#include <QFile>
#include <QSaveFile>
#include <QJsonDocument>
#include <QString>
#include <QJsonArray>
#include <QJsonValue>
#include <QByteArray>
#include <QIODevice>

using namespace NWSim;
std::shared_ptr<EndHost> Network::CreateEndHost(const std::string &address, float posX, float posY)
{
    //std::cout << "Attempting to create EndHost: " << address << " ... ";
    std::shared_ptr<EndHost> n = nullptr;
    // Check if address is already used?
    if (!FindNode(address))
    {
        n = std::make_shared<EndHost>(0.0, 0.0, address);
        _nodes.push_back(n);
        //std::cout << "Success!" << std::endl;
    }
    else
    {
        //std::cout << "Fail!" << std::endl;
    }
    return n;
}

std::shared_ptr<Router> Network::CreateRouter(const std::string &address, float posX, float posY)
{
    //std::cout << "Attempting to create Router: " << address << " ... ";
    std::shared_ptr<Router> n = nullptr;
    // Check if address is already used?
    if (!FindNode(address))
    {
        _isready = false; // we are not ready to simulate
        n = std::make_shared<Router>(0.0, 0.0, address);
        _nodes.push_back(n);
        //std::cout << "Success!" << std::endl;
    }
    else
    {
        //std::cout << "Fail!" << std::endl;
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

std::shared_ptr<Link> Network::FindLink(std::shared_ptr<Node> n1, std::shared_ptr<Node> n2)
{
    std::shared_ptr<Link> ret = nullptr;
    for (auto l : _links)
    {
        auto node1 = std::get<0>(l);
        auto node2 = std::get<1>(l);
        auto link = std::get<2>(l);
        // link can be found in either "orientation" in _links
        if (node1 == n1 && node2 == n2 ||
            node1 == n2 && node2 == n1)
        {
            ret = link;
            break;
        }
    }
    return ret;
}

void Network::RemoveNode(std::shared_ptr<Node> node)
{
    _isready = false; // we are not ready to simulate
    node->ClearReceivedQueue();
    node->ClearTransmitQueue();
    if (node->GetNodeType() == "Router")
    {
        auto r = std::static_pointer_cast<Router>(node);
        r->ClearRoutingTable();
    }
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
    _isready = false; // we are not ready to simulate
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
        throw std::logic_error("Error: Attempting to unlink invalid Nodes.");
    }
    _isready = false; // we are not ready to simulate
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
    struct sortableNode
    {
        int cost;
        std::shared_ptr<Node> node;
    };
    struct smallerSortableNode
    {
        bool operator()(const sortableNode &n1, const sortableNode &n2)
        {
            return n1.cost < n2.cost;
        }
    };
    // Initialize routing table to nullptr
    for (auto s : _nodes)
    {
        for (auto t : _nodes)
        {
            _routingTable[{s->network_interface.GetAddressStr(), t->network_interface.GetAddressStr()}].lock() = nullptr;
        }
    }

    // std::cout << "Generating routing table..." << std::endl;
    for (auto source : _nodes)
    {

        // std::cout << "Handling source: " << source->network_interface.GetAddressStr();

        std::map<std::shared_ptr<Node>, int> dist;
        std::map<std::shared_ptr<Node>, bool> visited;
        std::map<std::shared_ptr<Node>, std::shared_ptr<Node>> comes_from;
        for (auto target : _nodes)
        {
            // skip self to self
            if (source != target)
            {
                dist[target] = INT_MAX;
            }
        }
        dist[source] = 0;
        // std::cout << " - has " << dist.size() << " possible targets." << std::endl;

        std::priority_queue<sortableNode, std::vector<sortableNode>, smallerSortableNode> nxt;
        nxt.push({0, source});
        // std::cout << "Starting nxt loop..." << std::endl;
        while (!nxt.empty())
        {
            auto current = nxt.top().node;
            nxt.pop();
            // std::cout << "\tCurrent: " << current->network_interface.GetAddressStr() << std::endl;
            if (visited[current])
                continue;
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
                // std::cout << "\t\tneighbor: " << candidate_node->network_interface.GetAddressStr() <<
                //     "\twith newcost: " << candidate_newdist <<
                //     "\t and oldcost: " << dist[candidate_node] << std::endl;
                if (candidate_newdist < dist[candidate_node])
                {
                    dist[candidate_node] = candidate_newdist;
                    comes_from[candidate_node] = current;
                    nxt.push({-dist[candidate_node], candidate_node});
                }
            }
            // Set routing table
            for (auto x : comes_from)
            {
                _routingTable[std::make_pair(x.first->network_interface.GetAddressStr(), source->network_interface.GetAddressStr())] = x.second;
            }
        }
    }
}

void Network::AttachRoutingTable()
{
    for (auto node : _nodes)
    {
        if (node->GetNodeType() == "Router")
        {
            auto router = std::static_pointer_cast<Router>(node);
            router->SetRoutingTable(_routingTable);
        }
    }
}

void Network::SetEventTimeSizes()
{
    for (auto node : _nodes)
    {
        node->ClearReceivedQueue();
        node->ClearTransmitQueue();
        auto size = node->_connected.size();
        auto sim = std::static_pointer_cast<Simulatable>(node);
        sim->SetSize(size); // All nodes can have different sizes
    }
    for (auto l : _links)
    {
        auto link = std::get<2>(l);
        auto sim = std::static_pointer_cast<Simulatable>(link);
        sim->SetSize(2); // All links have event size 2, both packet directions of the link
    }
}

void Network::InitializeForSimulation()
{
    GenerateRoutingTable();
    AttachRoutingTable();
    SetEventTimeSizes();
    _isready = true; // we are finally ready to simulate
}

void Network::PrintRoutingTable(bool showAll) const
{
    std::cout << "Current routing table size: " << _routingTable.size() << std::endl;
    for (auto route : _routingTable)
    {
        auto current = route.first.first;
        auto target = route.first.second;
        auto sendto = (route.second.lock() == nullptr) ? "nullptr" : route.second.lock()->network_interface.GetAddressStr();
        if (!showAll)
        {
            if (route.second.lock() == nullptr)
                continue; // skip nullptrs
        }
        std::cout << "\tWhen at: " << current << "\tand target: " << target << "\tsend to: " << sendto << std::endl;
    }
}

void Network::RouteAllCurrentPackets()
{
    for (auto node : _nodes)
    {
        if (node->GetNodeType() == "Router")
        {
            auto router = std::static_pointer_cast<Router>(node);
            router->RunApplication();
        }
    }
}

bool Network::SimulateAllNodesAndLinks()
{
    if (IsRunnable())
    {
        // keep track of possible events
        bool ret = false;
        RouteAllCurrentPackets();
        for (auto node : _nodes)
        {
            ret = (node->Simulate()) ? true : ret;
        }
        for (auto l : _links)
        {
            auto link = std::get<2>(l);
            ret = (link->Simulate()) ? true : ret;
        }
        return ret;
    }
    else
    {
        return false;
    }
}

void Network::StartAllEndHosts()
{
    for (auto node : _nodes)
    {
        if (node->GetNodeType() == "EndHost")
        {
            auto eh = std::static_pointer_cast<EndHost>(node);
            eh->RunApplication();
        }
    }
}

void Network::PrintNetwork() const
{
    for (auto node : _nodes)
    {
        // EndHost 123.123.123.123
        // Router
        std::cout << std::right << std::setw(8) << node->GetNodeType() << " ";
        std::cout << std::left << std::setw(15) << node->network_interface.GetAddressStr();
        std::cout << " linked to: " << std::endl;
        for (auto neighbor : node->_connected)
        {
            auto n = neighbor.first.lock();
            std::cout << "\t";
            std::cout << std::right << std::setw(8) << node->GetNodeType() << " ";
            std::cout << std::left << std::setw(15) << node->network_interface.GetAddressStr();
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

void Network::PrintSimPlan() const
{
    for (auto node : _nodes)
    {
        if (node->GetNodeType() == "EndHost")
        {
            auto eh = std::static_pointer_cast<EndHost>(node);
            if (eh->network_interface.GetAddressStr() != eh->GetTargetAddress())
            {
                std::cout << std::left << std::setw(15) << eh->network_interface.GetAddressStr();
                std::cout << " sends ";
                std::cout << std::left << std::setw(5) << eh->GetPacketCount();
                std::cout << " to: ";
                std::cout << std::left << std::setw(15) << eh->GetTargetAddress();
                std::cout << std::endl;
            }
        }
    }
}

size_t Network::PrintPacketQueueStatuses(const size_t backtrack, const size_t timestep) const
{
    std::size_t linecount = 0;
    std::stringstream ss;
    if (backtrack != 0)
    {
        // ANSI escape number of lines.
        ss << "\x1b[" << backtrack << "A";
    }
    ss << "NODES:"
       << "\n";
    linecount++;
    for (auto node : _nodes)
    {
        // EndHost 123.123.123.123
        // Router
        ss << "\t";
        ss << std::right << std::setw(8) << node->GetNodeType() << " ";
        ss << std::left << std::setw(15) << node->network_interface.GetAddressStr();
        ss << " TX: ";
        ss << std::left << std::setw(5) << node->GetTransmitQueueLength();
        ss << " RX: ";
        ss << std::left << std::setw(5) << node->GetReceivedQueueLength();
        ss << "\n";
        linecount++;
    }
    ss << "LINKS:"
       << "\n";
    linecount++;
    for (auto l : _links)
    {
        auto n1 = std::get<0>(l);
        auto n2 = std::get<1>(l);
        auto link = std::get<2>(l);
        ss << "\t";
        ss << std::right << std::setw(15) << n1->network_interface.GetAddressStr();
        ss << " - ";
        ss << std::left << std::setw(15) << n2->network_interface.GetAddressStr();
        ss << " TX: ";
        ss << std::left << std::setw(5) << link->GetTransmissionQueueLength();
        ss << "\n";
        linecount++;
    }
    if (timestep > 0)
    {
        ss << "Current timestep: " << timestep << "\n";
        linecount++;
    }
    std::string s = ss.str();
    std::cout << s << std::flush;
    return linecount;
}

void Network::Read(const QJsonObject &json) {
    if (json.contains("nodes") && json["nodes"].isArray()) {
        for (auto n : json["nodes"].toArray()) {
            QJsonObject node = n.toObject();
            if (node.contains("address") && node["address"].isString() &&
                node.contains("type") && node["type"].isString() &&
                node.contains("position") && node["position"].isObject()) 
            {
                std::string address = node["address"].toString().toStdString();
                std::string type = node["type"].toString().toStdString();
                QJsonObject pos = node["position"].toObject();

                if (type == "EndHost") {
                    std::shared_ptr<EndHost> host = CreateEndHost(address, pos["x"].toDouble(), pos["y"].toDouble());
                    host->SetTargetAddress(node["targetAddress"].toString().toStdString());
                    host->SetPacketCount(node["packetCount"].toInt());
                }
                else if (type == "Router") 
                    CreateRouter(address, pos["x"].toDouble(), pos["y"].toDouble());
            }
        }
    }

    if (json.contains("links") && json["links"].isArray()) {
        for (auto l : json["links"].toArray()) {
            QJsonObject link = l.toObject();
            if (link.contains("address_1") && link["address_1"].isString() &&
                link.contains("address_2") && link["taddress_2"].isString() &&
                link.contains("transmissionSpeed") &&
                link.contains("propagationDelay")) 
            {
                std::string address_1 = link["address_1"].toString().toStdString();
                std::string address_2 = link["address_2"].toString().toStdString();
                uint32_t transmission_speed = link["transmissionSpeed"].toInt();
                uint32_t propagation_delay = link["propagationDelay"].toInt();

                std::shared_ptr<Node> n1 = FindNode(address_1);
                std::shared_ptr<Node> n2 = FindNode(address_2);
                std::shared_ptr<Link> link = LinkNodes(n1, n2);
                link->SetTransmissionSpeed(transmission_speed);
                link->SetPropagationDelay(propagation_delay);   
            }
        }
    }
}

void Network::Write(QJsonObject &json) {
    QJsonArray nodes_arr;
    QJsonArray links_arr;

    for (auto n : _nodes) {
        QJsonObject nodeObject;
        const QString address = QString::fromStdString(n->network_interface.GetAddressStr());
        const QString type = QString::fromStdString(n->GetNodeType()); 
        Position pos = n->GetPosition();

        nodeObject["address"] = address; // has to be unique
        nodeObject["type"] = type;
        nodeObject["position"] = QJsonObject {{"x", pos.posX }, {"y", pos.posY }};

        if (type == "EndHost") {
            const QString target = QString::fromStdString(std::static_pointer_cast<EndHost>(n)->GetTargetAddress());
            int count = std::static_pointer_cast<EndHost>(n)->GetPacketCount();
            nodeObject["targetAddress"] = target;
            nodeObject["packetCount"] = count;
        }
        nodes_arr.push_back(nodeObject);
    }

    for (auto l : _links) {
        std::shared_ptr node_1 = std::get<0>(l);
        std::shared_ptr node_2 = std::get<1>(l);
        std::shared_ptr link = std::get<2>(l);
        QJsonObject linkObject;

        const QString address_1 = QString::fromStdString(node_1->network_interface.GetAddressStr());
        const QString address_2 = QString::fromStdString(node_2->network_interface.GetAddressStr());
        
        linkObject["address_1"] = address_1;
        linkObject["address_2"] = address_2;
        linkObject["transmissionSpeed"] = (int)link->GetTransmissionSpeed();
        linkObject["propagationDelay"] = (int)link ->GetPropagationDelay();
        links_arr.push_back(linkObject);
    }

    json["nodes"] = nodes_arr;
    json["links"] = links_arr;
}

bool Network::Load(std::string fileName, fileType saveFormat) {
    QString fn = QString::fromStdString(saveFormat == Json
        ? fileName + ".json"
        : fileName + ".dat");
    QFile lf(fn);

    if (!lf.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray data = lf.readAll();
    QJsonDocument loadDoc(saveFormat == Json
        ? QJsonDocument::fromJson(data)
        : QJsonDocument::fromBinaryData(data));

    Read(loadDoc.object());

    return true;
}

bool Network::Save(const std::string fileName, fileType saveFormat) {

    QString fn = QString::fromStdString(saveFormat == Json
    ? fileName + ".json"
    : fileName + ".dat");

    QSaveFile sf(fn);

    if (!sf.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonObject networkObject;
    Write(networkObject);
    QJsonDocument saveDoc(networkObject);
    sf.write(saveFormat == Json
        ? saveDoc.toJson()
        : saveDoc.toBinaryData());
    sf.commit();

    return true;
}