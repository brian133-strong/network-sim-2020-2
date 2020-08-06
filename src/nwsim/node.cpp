#include <queue>
#include "node.hpp"

Packet Node::GetTransmitPacket()
{
    Packet p;
    if (GetTransmitQueueLength() > 0)
    {
        p = _transmit.front().first;
        _transmit.pop();
    }
    return p;
}

void Node::ReceivePacket(Packet p)
{
    _receive.push(p);
}

void Node::AddTransmitPacket(Packet p, std::shared_ptr<Node> n)
{
    if (p.GetTimeToLive() > 0)
    {
        _transmit.push(std::make_pair(p,n));
    }
}

void Node::WriteToJSON(QJsonObject &json) const {
    const QString address = QString::fromStdString( this->network_interface.GetAddressStr() );
    //QString application = QString::fromStdString( _app._placeholder ); 

    json["address"] = QJsonValue(address); // has to be unique
    json["application"] = "Yay, I'm an application!"; // TODO
    json["position"] = QJsonObject { { "x", _pos.posX }, { "y", _pos.posY } }; // has to be unique
    
}

void Node::ReadFromJSON(QJsonObject &json) {

    if (json.contains("address") && json["address"].isString())
        this->network_interface.SetAddress( json["address"].toString().toStdString()); // TODO: Handle invalid input

    if (json.contains("application") && json["application"].isString())
        // Set node application
        ;

    if (json.contains("position") && json["position"].isObject()) {
        QJsonObject pos = json["position"].toObject();
        this->SetPosition(pos["x"].toInt(), pos["y"].toInt());
    }
        

}