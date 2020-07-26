#pragma once
#include <queue>
#include "packet.hpp"
#include "node.hpp"

class Link {
public:

private:
    // Queue currently being transmitted, holds Packet to transmit and the "address" as the Node which should receive it
    std::queue<std::pair<Packet, Node*>> _transmission_queue;
    unsigned int _transmission_speed; // in bits / second
    unsigned int _propagation_delay;  // in ms
};