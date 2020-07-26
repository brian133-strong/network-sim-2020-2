#pragma once
#include <vector>
#include "networkinterface.hpp"
#include "application.hpp"
#include "link.hpp"
class Node {
public:
    Node() { }
    ~Node() { }
private:
    // holds all connections to other nodes and the actual Link object;
    std::vector<std::pair<Link, Node*>> _connected_to;
    Application _app;
    NetworkInterface _nic;
};