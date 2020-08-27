#pragma once
#include <vector>
#include <map>
#include "node.hpp"
#include "link.hpp"

#include <QJsonObject>

namespace NWSim
{
    //class Node;
    //class Link;
    enum fileType {binary, Json};

    class Network
    {
    public:
        Network() {}
        // Constructor for Network, initializes NW from a file to be read from (JSON by default).
        Network(const std::string fn, fileType sf = Json) : Network() {
            if (!Load(fn, sf)) {
                std::cout << "Failed to load network from file." << std::endl;
                delete this;
            }
        }
        ~Network() {
            _routingTable.clear();
        }
        /* 
         * Creates new nodes. If non-unique IP is used, a nullptr is returned and nothing is added to _nodes.
         */
        std::shared_ptr<EndHost> CreateEndHost(const std::string &address = "0.0.0.0", float posX = 0.0, float posY = 0.0);
        std::shared_ptr<Router> CreateRouter (const std::string &address = "0.0.0.0", float posX = 0.0, float posY = 0.0);
        // Returns nullptr if node was not found
        std::shared_ptr<Node> FindNode(const std::string &address) const;
        // Returns nullptr if link was not found
        std::shared_ptr<Link> FindLink(std::shared_ptr<Node> n1, std::shared_ptr<Node> n2);
        void RemoveNode(std::shared_ptr<Node> n);
        // Links two nodes with a bidirectional link, link objects ptr added in _links as well.
        std::shared_ptr<Link> LinkNodes(std::shared_ptr<Node> n1, std::shared_ptr<Node> n2);
        void RemoveLink(std::shared_ptr<Node> n1, std::shared_ptr<Node> n2);

        size_t size() const { return _nodes.size(); }

        // There needs to be some preprocessing done before we can simulate the network (i.e. routing table)
        bool IsRunnable() const { return _isready; }
        // Must be run before simulation can be run, sets _isready
        void InitializeForSimulation();
        // For all EndHosts in _nodes, do RunApplication()
        void StartAllEndHosts();
        // For all Routers in _nodes, do RunApplication()
        void RouteAllCurrentPackets();
        // For all node in _nodes and link in _links do Simulate(). Returns false when no new events were detected.
        bool SimulateAllNodesAndLinks();
        void PrintNetwork() const;
        void PrintSimPlan() const;
        // Prints node and link queue packet counts. parameter tells how many chars backwards to \b to reprint. Returns charcount
        size_t PrintPacketQueueStatuses(const size_t backtrack = 0, const size_t timestep = 0) const;
        void PrintRoutingTable(bool showAll = false) const;

        bool Save(std::string fileName, fileType saveFormat);
    private:
        void Read(const QJsonObject &json);
        bool Load(const std::string fileName, fileType saveFormat);
        void Write(QJsonObject &json);
        // Dijkstra from each source to each target
        void GenerateRoutingTable();
        // Naive copy to all router nodes
        void AttachRoutingTable();
        // Go through all nodes and links and set their respective Simulatable::evt_times size
        // Also empties all Received and Transmit queues.
        void SetEventTimeSizes();
        bool _isready = false;
        // Routing table of <<CurrentNode, TargetAddress>, IntermediateTargetNode>
        // Keeping CurrentNode and TargetAddress as strings for readability
        std::map<std::pair<std::string,std::string>, std::weak_ptr<Node>> _routingTable;
        std::vector<std::shared_ptr<Node>> _nodes;
        // Vector of links with the nodes that it binds together.
        std::vector<std::tuple<
            std::shared_ptr<Node>, std::shared_ptr<Node>, std::shared_ptr<Link>>>
            _links;
    };
} // namespace NWSim