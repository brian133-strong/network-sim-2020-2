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

        ~Network() {}
        /* 
         * Creates new nodes. If non-unique IP is used, a nullptr is returned and nothing is added to _nodes.
         */
        std::shared_ptr<EndHost> CreateEndHost(const std::string &address = "0.0.0.0", float posX = 0.0, float posY = 0.0);
        std::shared_ptr<Router> CreateRouter (const std::string &address = "0.0.0.0", float posX = 0.0, float posY = 0.0);
        // Returns nullptr if node was not found
        std::shared_ptr<Node> FindNode(const std::string &address) const;
        void RemoveNode(std::shared_ptr<Node> n);
        // Links two nodes with a bidirectional link, link objects ptr added in _links as well.
        std::shared_ptr<Link> LinkNodes(std::shared_ptr<Node> n1, std::shared_ptr<Node> n2);
        void RemoveLink(std::shared_ptr<Node> n1, std::shared_ptr<Node> n2);

        size_t size() const { return _nodes.size(); }

        // There needs to be some preprocessing done before we can simulate the network (i.e. routing table)
        bool IsRunnable() const;
        // 
        void GenerateRoutingTable();
        void PrintRoutingTable();

        bool Save(std::string fileName, fileType saveFormat);

    private:
        void Read(const QJsonObject &json);
        bool Load(const std::string fileName, fileType saveFormat);
        void Write(QJsonObject &json);

        // Routing table of <<CurrentNode, TargetAddress>, IntermediateTargetNode>
        // Keeping CurrentNode and TargetAddress as strings for readability
        std::map<std::pair<std::string,std::string>, std::shared_ptr<Node>> _routingTable;
        std::vector<std::shared_ptr<Node>> _nodes;
        // Vector of links with the nodes that it binds together.
        std::vector<std::tuple<
            std::shared_ptr<Node>, std::shared_ptr<Node>, std::shared_ptr<Link>>>
            _links;
    };
} // namespace NWSim