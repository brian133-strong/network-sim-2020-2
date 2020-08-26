#include <iostream>
#include <iomanip>
#include <algorithm>
#include <memory>
#include <vector>
#include <map>
#include "network.hpp"
#include "node.hpp"
#include "link.hpp"
#include "address.hpp"
#include "packet.hpp"
#include "simulatable.hpp"
#include "./tests/testroutines.hpp"

// Map of valid commands and command related options
std::map<std::string, std::vector<std::string>> ValidCommands(const std::string &type = "root")
{
    std::map<std::string, std::vector<std::string>> ret;
    ret["help"] = {"", "edit", "sim"};
    ret["list"] = {"v"};
    ret["exit"] = {};
    ret["quit"] = {};
    ret["q"] = {};
    if (type == "root")
    {
        ret["save"] = {};
        ret["load"] = {};
        ret["add"] = {"e", "r"};
        ret["rem"] = {};
        ret["link"] = {};
        ret["unlink"] = {};
        ret["edit"] = {};
        ret["tests"] = {};
    }
    if (type == "editnode")
    {
        ret["set"] = {"target", "count", "address"};
    }
    if (type == "editlink")
    {
        ret["set"] = {"ts", "pd"};
    }
    return ret;
}

//print line
void println(const std::string &s)
{
    std::cout << s << std::endl;
}

void PrintUsageLine(const std::string &cmd, const std::string &options, const std::string &desc)
{
    int W_CMD = 8;
    int W_PAR = 20;
    std::string sep = " - ";
    if (cmd == "" && options == "")
        sep = "   ";
    std::cout << std::right << std::setw(W_CMD) << cmd + " " << std::left << std::setw(W_PAR) << options << sep << desc << std::endl;
}

void PrintUsage(const std::string &mode = "")
{
    if (mode == "")
    {
        PrintUsageLine("help", "", "Print this manual.");
        PrintUsageLine("", "<mode>", "Prints mode specific usage.");
        println("");

        PrintUsageLine("save", "<filename>", "Save current network configuration as JSON to given file.");
        println("");

        PrintUsageLine("load", "<filename>", "Discard current network configuration and load from specified file.");
        println("");

        PrintUsageLine("exit", "", "Exit program.");
        println("");

        PrintUsageLine("list", "", "Lists all current nodes and what other nodes they are linked to.");
        PrintUsageLine("", "v", "Verbose print of all parameters");
        println("");

        PrintUsageLine("add", "e|r <address>", "Adds an [e]ndhost or [r]outer with given address.");
        PrintUsageLine("", "", "NOTE: Address must be IP format and unique in network.");
        println("");

        PrintUsageLine("rem", "<address>", "Removes an endhost or router that matches the given address, and severs affected links.");
        PrintUsageLine("", "", "If node doesn't exist, nothing happens.");
        println("");

        PrintUsageLine("link", "<address> <address>", "Links given nodes, if they exist.");
        println("");
        PrintUsageLine("unlink", "<address> <address>", "Unlinks given nodes, if they are currently linked.");
        println("");

        PrintUsageLine("edit", "<address>", "Enter node (endhost, router) edit mode.");
        PrintUsageLine("", "<address> <address>", "Enter link edit mode.");
        println("");

        PrintUsageLine("tests", "", "Prinst result of all tests and exit program.");
        println("");
    }
    else if (mode == "editnode" || mode == "editlink" || mode == "edit")
    {
        PrintUsageLine("help", "", "Print this help for edit mode.");
        println("");

        PrintUsageLine("exit", "", "Exit edit mode.");
        println("");

        PrintUsageLine("list", "", "List all changeable parameters.");
        println("");
        if (mode == "editnode" || mode == "edit")
        {
            PrintUsageLine("set", "address <address>", "Changes this nodes to use the given address.");
            PrintUsageLine("", "", "NOTE: Address must be IP format and unique in network.");
            println("");
            PrintUsageLine("set", "target <address>", "Requires endhost source and endhost target. Sets target address for simulation.");
            PrintUsageLine("", "", "NOTE: Address must be IP format and exist in current network.");
            PrintUsageLine("", "", "      If set to self, no packets sent in simulation.");
            println("");
            PrintUsageLine("set", "count <integer>", "Requires endhost. Sets amount of packets sent to target for smiulation.");
            println("");
        }
        if (mode == "editlink" || mode == "edit")
        {
            PrintUsageLine("set", "ts <integer>", "Sets links transmission speed to given value (timeunit).");
            PrintUsageLine("", "", "Value determines the interval at which new packets can be transmitted to the link.");
            println("");
            PrintUsageLine("set", "pd <integer>", "Sets links propagation delay to given value (timeunit / byte).");
            PrintUsageLine("", "", "Value determines the time it takes for a packet to travel accross the link.");
            PrintUsageLine("", "", "time = propagation_delay * packet_size");
            println("");
        }
    }
    else if (mode == "sim")
    {
        println("sim help TODO");
    }
    else
    {
        println("Sorry, no help for '" + mode + "'");
    }
}

void PrintInvalidCommand()
{
    println("Invalid command, type 'help' to see usage.");
}

std::vector<std::string> ParseUserInput(std::string userin, const std::string &type = "root")
{
    auto cmds = ValidCommands(type);
    std::string buf;
    std::stringstream ss(userin);
    std::vector<std::string> split;
    while (ss >> buf)
        split.push_back(buf);

    try
    {
        // map.at() throws if nothing exists, this is what we use to determine validity
        auto options = cmds.at(split[0]);
        // Need better way to parse out possible parameters here
        // auto match = std::find(options.begin(), options.end(),split[1]);
        // if(match == options.end() && options.size() > 0)
        //     split = {}; // null out the commands to indicate invalidity
    }
    catch (const std::exception &e)
    {
        //std::cerr << e.what() << '\n';
        split = {};
    }
    return split;
}

void PrintParameterLine(const std::string &param, const std::string &value, const std::string &desc)
{
    int W_PAR = 8;
    int W_VAL = 20;
    std::string sep = " - ";
    if (param == "" && value == "")
        sep = "   ";
    std::cout << std::right << std::setw(W_PAR) << param + ": " << std::left << std::setw(W_VAL) << value << sep << desc << std::endl;
}
void EditNodeProcedure(std::shared_ptr<NWSim::Node> node, std::shared_ptr<NWSim::Network> nw)
{
    auto commands = ValidCommands("editnode");
    if (node == nullptr)
        return;
    std::string userin = "";
    while (true)
    {
        auto ntype = node->GetNodeType();
        std::cout << "[Edit " << ntype << "]>";
        std::getline(std::cin, userin);
        if (userin.length() == 0)
            continue;
        auto parsed = ParseUserInput(userin, "editnode");
        if (parsed.size() == 0)
        {
            PrintInvalidCommand();
            continue;
        }
        auto options = commands[parsed[0]];
        if (parsed[0] == "exit" || parsed[0] == "quit" || parsed[0] == "q")
            break;
        if (parsed[0] == "help")
        {
            PrintUsage("editnode");
        }
        if (parsed[0] == "list")
        {
            PrintParameterLine("address", node->network_interface.GetAddressStr(), "Current IP address.");
            if (ntype == "EndHost")
            {
                auto eh = std::static_pointer_cast<NWSim::EndHost>(node);
                PrintParameterLine("target", eh->GetTargetAddress(), "Simulation target address.");
                PrintParameterLine("count", std::to_string(eh->GetPacketCount()), "Simulation packet count to be sent to target address.");
                PrintParameterLine("", "", "Clamped between " + std::to_string(eh->MINPACKETS) + " and " + std::to_string(eh->MAXPACKETS) + ".");
            }
            if (ntype == "Router")
            {
                // No router specific parameters
            }
        }
        if (parsed[0] == "set")
        {
            // Validity checks
            if (!parsed.size() == 3)
            {
                PrintInvalidCommand();
                continue;
            }
            auto match = std::find(options.begin(), options.end(), parsed[1]);
            if (match == options.end())
            {
                PrintInvalidCommand();
                continue;
            }
            // Router has only address editable
            if (ntype == "Router" && parsed[1] != "address")
            {
                PrintInvalidCommand();
                continue;
            }
            // checks end
            if (parsed[1] == "address" || parsed[1] == "target")
            {
                // Validity checks
                try
                {
                    // this will throw if the address is invalid
                    NWSim::AddressStrToInt(parsed[2]);
                }
                catch (const std::exception &e)
                {
                    println(e.what());
                    continue;
                }
                auto tmp = nw->FindNode(parsed[2]);
                if (tmp != nullptr)
                {
                    printline("Node with that address already exists.");
                    continue;
                }
                // checks end
                if (parsed[1] == "address")
                {
                    node->network_interface.SetAddress(parsed[2]);
                    printline("Successfully changed node address to " + parsed[2]);
                    if (node->GetNodeType() == "EndHost")
                    {
                        auto eh = std::static_pointer_cast<NWSim::EndHost>(node);
                        eh->SetTargetAddress(parsed[2]);
                    }
                    continue;
                }
                if (parsed[1] == "target")
                {
                    if (tmp->GetNodeType() != "EndHost")
                    {
                        printline("Target must be an endhost.");
                        continue;
                    }
                    auto eh = std::static_pointer_cast<NWSim::EndHost>(node);
                    eh->SetTargetAddress(parsed[2]);
                    printline("Successfully changed target address to " + parsed[2]);
                    continue;
                }
            }
            else if (parsed[1] == "count")
            {
                int val = 0;
                try
                {
                    val = std::stoi(parsed[2], nullptr, 10);
                }
                catch (const std::exception &e)
                {
                    PrintInvalidCommand();
                    continue;
                }
                auto eh = std::static_pointer_cast<NWSim::EndHost>(node);
                eh->SetPacketCount(val);
                printline("Successfully changed packet count to " + parsed[2]);
            }
        }
    }
}

void EditLinkProcedure(std::shared_ptr<NWSim::Link> link)
{
    auto commands = ValidCommands("editlink");
    if (link == nullptr)
        return;
    std::string userin = "";
    while (true)
    {
        std::cout << "[Edit Link]>";
        std::getline(std::cin, userin);
        if (userin.length() == 0)
            continue;
        auto parsed = ParseUserInput(userin, "editlink");
        if (parsed.size() == 0)
        {
            PrintInvalidCommand();
            continue;
        }
        auto options = commands[parsed[0]];
        if (parsed[0] == "exit" || parsed[0] == "quit" || parsed[0] == "q")
            break;
        if (parsed[0] == "help")
        {
            PrintUsage("editlink");
        }
        if (parsed[0] == "list")
        {
            PrintParameterLine("ts", std::to_string(link->GetTransmissionSpeed()), "Transmission speed (ts) determines the interval at which new packets can be transmitted to the link.");
            PrintParameterLine("pd", std::to_string(link->GetPropagationDelay()), "Propagation delay (pd) determines the time it takes for a packet to travel accross the link.");
            PrintParameterLine("", "", "time = propagation_delay * packet_size");
        }
        if (parsed[0] == "set")
        {
            // Validity checks
            if (!parsed.size() == 3)
            {
                PrintInvalidCommand();
                continue;
            }
            auto match = std::find(options.begin(), options.end(), parsed[1]);
            if (match == options.end())
            {
                PrintInvalidCommand();
                continue;
            }
            if (parsed[1] == "pd")
            {
                int val = 0;
                try
                {
                    val = std::stoi(parsed[2], nullptr, 10);
                }
                catch (const std::exception &e)
                {
                    PrintInvalidCommand();
                    continue;
                }
                link->SetPropagationDelay(val);
                printline("Successfully changed propagation delay to " + parsed[2]);
            }
            else if (parsed[1] == "ts")
            {
                int val = 0;
                try
                {
                    val = std::stoi(parsed[2], nullptr, 10);
                }
                catch (const std::exception &e)
                {
                    PrintInvalidCommand();
                    continue;
                }
                link->SetTransmissionSpeed(val);
                printline("Successfully changed transmission speed to " + parsed[2]);
            }
        }
    }
}

int main(void)
{

    std::shared_ptr<NWSim::Network> nw = std::make_shared<NWSim::Network>();
    auto commands = ValidCommands();
    std::string userin = "";
    println("=== nwsim v1 ===");
    while (true)
    {
        std::cout << ">";
        std::getline(std::cin, userin);
        if (userin.length() == 0)
            continue;
        auto parsed = ParseUserInput(userin);
        if (parsed.size() == 0)
        {
            PrintInvalidCommand();
            continue;
        }
        auto options = commands[parsed[0]];
        if (parsed[0] == "help")
        {
            std::string help = "";
            if (parsed.size() > 1)
            {
                help = parsed[1];
            }
            PrintUsage(help);
        }
        else if (parsed[0] == "exit" || parsed[0] == "quit" || parsed[0] == "q")
        {
            println("Exiting, any unsaved changes are lost.");
            break;
        }
        else if (parsed[0] == "tests")
        {
            PacketTestRoutine();
            AddressTestRoutine();
            NetworkInterfaceTestRoutine();
            NodeTestRoutine();
            EndHostTestRoutine();
            RouterTestRoutine();
            LinkTestRoutine();
            NetworkTestRoutine();
        }
        else if (parsed[0] == "add")
        {
            // Validity checks
            if (!parsed.size() == 3)
            {
                PrintInvalidCommand();
                continue;
            }
            auto match = std::find(options.begin(), options.end(), parsed[1]);
            if (match == options.end())
            {
                PrintInvalidCommand();
                continue;
            }
            try
            {
                // this will throw if the address is invalid
                NWSim::AddressStrToInt(parsed[2]);
                auto n = nw->FindNode(parsed[2]);
                if (n != nullptr)
                {
                    println("Node with that address already exists.");
                    continue;
                }
            }
            catch (const std::exception &e)
            {
                println(e.what());
                continue;
            }
            // Checks end
            if (parsed[1] == "e")
            {
                auto n = nw->CreateEndHost(parsed[2]);
                println("Added EndHost " + parsed[2]);
                EditNodeProcedure(n, nw);
            }
            if (parsed[1] == "r")
            {
                auto n = nw->CreateRouter(parsed[2]);
                println("Added Router " + parsed[2]);
                EditNodeProcedure(n, nw);
            }
        }
        else if (parsed[0] == "rem")
        {
            // Validity checks
            if (!parsed.size() == 2)
            {
                PrintInvalidCommand();
                continue;
            }
            try
            {
                // this will throw if the address is invalid
                NWSim::AddressStrToInt(parsed[1]);
            }
            catch (const std::exception &e)
            {
                println(e.what());
                continue;
            }
            auto n = nw->FindNode(parsed[1]);
            if (n == nullptr)
            {
                println("No node with address " + parsed[1]);
                continue;
            }
            else //checks end
            {
                nw->RemoveNode(n);
                printline("Node " + parsed[1] + " removed successfully.");
                continue;
            }
        }
        else if (parsed[0] == "link" || parsed[0] == "unlink")
        {
            // We can use same logic for both link and unlink to validate addresses
            // validity checks
            if (!parsed.size() == 3)
            {
                PrintInvalidCommand();
                continue;
            }
            std::shared_ptr<NWSim::Node> n1 = nullptr;
            std::shared_ptr<NWSim::Node> n2 = nullptr;
            try
            {
                NWSim::AddressStrToInt(parsed[1]);
                NWSim::AddressStrToInt(parsed[2]);
            }
            catch (const std::exception &e)
            {
                println(e.what());
                continue;
            }
            n1 = nw->FindNode(parsed[1]);
            n2 = nw->FindNode(parsed[2]);
            try
            {
                // checks end
                if (parsed[0] == "link")
                {
                    auto l = nw->LinkNodes(n1, n2);
                    println("Linked nodes " + parsed[1] + " - " + parsed[2]);
                    EditLinkProcedure(l);
                    continue;
                }
                else if (parsed[0] == "unlink")
                {
                    // RemoveLink does not actually check if the two nodes are linked,
                    // but calling NWSim::Node.DisconnectFromNode does nothing if they arent linked
                    nw->RemoveLink(n1, n2);
                    continue;
                }
            }
            catch (const std::exception &e)
            {
                println(e.what());
                continue;
            }
        }
        else if (parsed[0] == "edit")
        {
            if (parsed.size() == 2)
            {
                try
                {
                    NWSim::AddressStrToInt(parsed[1]);
                }
                catch (const std::exception &e)
                {
                    println(e.what());
                    continue;
                }
                auto n = nw->FindNode(parsed[1]);
                if (n == nullptr)
                {
                    println("No node with address " + parsed[1]);
                    continue;
                }
                else
                {
                    EditNodeProcedure(n, nw);
                }
            }
            else if (parsed.size() == 3)
            {
                try
                {
                    NWSim::AddressStrToInt(parsed[1]);
                    NWSim::AddressStrToInt(parsed[2]);
                }
                catch (const std::exception &e)
                {
                    println(e.what());
                    continue;
                }
                auto n1 = nw->FindNode(parsed[1]);
                auto n2 = nw->FindNode(parsed[2]);
                auto l = nw->FindLink(n1, n2);
                if (l == nullptr)
                {
                    println("No link between " + parsed[1] + " - " + parsed[2]);
                    continue;
                }
                else
                {
                    EditLinkProcedure(l);
                }
            }
            else
            {
                PrintInvalidCommand();
                continue;
            }
        }
        else if (parsed[0] == "list")
        {
            nw->PrintNetwork();
            continue;
        }
        else if (parsed[0] == "save")
        {
            printline("TODO");
            // TODO: Save current network as JSON file
        }
        else if (parsed[0] == "load")
        {
            printline("TODO");
            // TODO: drop current network and load from file instead.
            // nw = nullptr;
            // json = ReadFileETC(parsed[1]);
            try
            {
                // nw = std::make_shared<NWSim::Network>(json);
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else if (parsed[0] == "sim")
        {
            // TODO: the actual simulation program...
            printline("TODO");
        }
    }

    return 0;
}
