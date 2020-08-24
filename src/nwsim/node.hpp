#pragma once
#include <vector>
#include <queue>
#include <list>
#include <memory>
#include <iostream>
#include "networkinterface.hpp"
#include "link.hpp"
namespace NWSim
{
    // Need to forward declare Link in order to compile
    class Link;
    // pixel position values
    struct Position
    {
        float posX;
        float posY;
    };


    // Node as base class for end-hosts and routers.
    // TODO: Convert Node to abstract, only allow generating end-hosts and routers
    class Node
    {
    public:
        Node(const std::string &type = "DEFAULT") : _pos({0, 0}), _nodetype(type) {}
        ~Node() {}
        // Create node at position X,Y
        Node(float posX, float posY) : Node() { SetPosition(posX, posY); }
        // Create node at position X,Y with given IP
        Node(float posX, float posY, std::string address) : Node()
        {
            SetPosition(posX, posY);
            network_interface.SetAddress(address);
        }
        /*
         * GUI methods
         */
        const std::string GetNodeType() { return _nodetype; }
        // Get Node position on GUI
        Position GetPosition() const { return _pos; }
        // Set Node position on GUI
        void SetPosition(float posX, float posY) { _pos = {posX, posY}; }
        /*
         * Network helpers
         */
        void ConnectToNode(std::shared_ptr<Node> n, std::shared_ptr<Link> l);
        bool IsConnectedTo(std::shared_ptr<Node> n) const;
        // Removes connection from _connected only.
        void DisconnectFromNode(std::shared_ptr<Node> n);

        /*
         * Comparisons 
         */
        // comparison between two nodes facilitated by the IP address
        friend bool operator==(const std::shared_ptr<Node> n1, const std::shared_ptr<Node> n2)
        {
            return n1->network_interface.GetAddressInt() == n2->network_interface.GetAddressInt();
        }
        // Compare node directly against an IP address string
        friend bool operator==(const std::shared_ptr<Node> n, const std::string &address)
        {
            try // check if address is valid format
            {
                return NWSim::AddressStrToInt(address) == n->network_interface.GetAddressInt();
            }
            catch (const std::exception &e) // above throws logic_error if wrong string format for IP
            {
                std::cerr << e.what() << std::endl;
                return false;
            }
        }
        void RunApplication();
        // Returns time when next packet from this node can be added to the link, if 0, no packets exist.
        uint32_t MoveTopTransmitPacketToLink();

        size_t GetTransmitQueueLength() const { return _transmit.size(); }
        void AddTransmitPacket(Packet p, std::shared_ptr<Node> n);
        void ReceivePacket(Packet p);
        std::queue<Packet> GetReceivedPackets() const { return _receive; }

        // If address needs to be changed, check if it is unique against the Network's _nodes vector first.
        NetworkInterface network_interface;
        // Vector of connected nodes. Using std::weak_ptr to not end up with memory management issues.
        std::vector<std::pair<std::weak_ptr<Link>, std::weak_ptr<Node>>> _connected;

    protected:
        // queue of packets to be sent to a link, keep track of where to send if multiple connected nodes
        std::queue<std::pair<Packet, std::shared_ptr<Node>>> _transmit;
        // queue of packets received from a link
        std::queue<Packet> _receive;
    private:
        const std::string _nodetype;
        // positions for drawing the node on the GUI
        Position _pos;
    };

    class Router : public Node
    {
    public:
        Router() : Node("Router") {}
        Router(float posX, float posY) : Router() { SetPosition(posX, posY); }
        Router(float posX, float posY, std::string address) : Router()
        {
            SetPosition(posX, posY);
            // TODO: does this break in GUI?
            if (!network_interface.SetAddress(address))
            {
                delete this;
            }
        }
        ~Router() {}
        /*
         * Router is supposed to route packets between end-hosts.
         * Check's receive-queue for any packets:
         *  - if packet is going to End-host that this Router is connected to, put to correct link.
         *  - if not connected to correct End-host, refer to Routing Table for which Link to place packet on.
         */
        void RunApplication();

    private:
    };
    class EndHost : public Node
    {
    public:
        // Guards against dumb packet counts crashing the application
        const uint32_t MAXPACKETS = 100;
        const uint32_t MINPACKETS = 1;

        EndHost() : Node("EndHost") 
        {
            SetTargetAddress(network_interface.GetAddressStr()); // set inital target address to self
            SetPacketCount(MINPACKETS);
        }
        EndHost(float posX, float posY) : EndHost() 
        { 
            SetPosition(posX, posY); 
            SetTargetAddress(network_interface.GetAddressStr()); // set inital target address to self
            SetPacketCount(MINPACKETS);
        }
        EndHost(float posX, float posY, std::string address) : EndHost()
        {
            SetPosition(posX, posY);
            // TODO: does this break in GUI?
            if (!network_interface.SetAddress(address))
            {
                delete this;
            }
            SetTargetAddress(network_interface.GetAddressStr()); // set inital target address to self
            SetPacketCount(MINPACKETS);
        }
        ~EndHost() {}
        void RunApplication();
        // returns true if succesfully changed adr (format can fail)
        bool SetTargetAddress(const std::string& address);
        const std::string GetTargetAddress() const { return _targetAddress; }
        void SetPacketCount(const uint32_t count);
        const uint32_t GetPacketCount() const { return _packetCount; }
        
    private:
        std::string _targetAddress;
        uint32_t _packetCount;
        const std::string _defaultMsg = "Test";

        // Naive approach, simply generates n packets with varying packet size 
        std::list<Packet> GenerateRandomPackets() const;
        std::list<Packet> GeneratePackets() const;
        // Gets target node for first jump out of this end-host
        std::shared_ptr<Node> GetTargetNode() const;

    };

} // namespace NWSim