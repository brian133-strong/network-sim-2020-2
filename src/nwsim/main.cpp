#include <iostream>
#include <exception>
#include "address.hpp"
#include "packet.hpp"
#include "networkinterface.hpp"
//print line
void println(const std::string& s) {
    std::cout << s << std::endl;
}

int main(void) {
    println("=== Testing NetworkInterface ===");
    std::cout << "Default constructor:" << std::endl;
    NetworkInterface n1;
    std::cout << "adr: '" << n1.GetAddressStr() << "', '" << n1.GetAddressInt() << "'" << std::endl;
    
    std::cout << "Setting address to '000.111.222.000', this should pass" << std::endl;
    try 
    {
        n1.SetAddress("000.111.222.000");
    }
    catch (const std::logic_error &e)
    {
        std::cout << "logic_error:" << std::endl;
        std::cout << e.what() << std::endl;
    }
    std::cout << "adr: '" << n1.GetAddressStr() << "', '" << n1.GetAddressInt() << "'" << std::endl;
    
    std::cout << "Setting address to '555.111.222.000', this should throw" << std::endl;
    try 
    {
        n1.SetAddress("555.111.222.000");
    }
    catch (const std::logic_error &e)
    {
        std::cout << "logic_error:" << std::endl;
        std::cout << e.what() << std::endl;
    }
    std::cout << "adr: '" << n1.GetAddressStr() << "', '" << n1.GetAddressInt() << "'" << std::endl;
    
    std::cout << "Setting address to '51289075', this should throw" << std::endl;
    try 
    {
        n1.SetAddress("51289075");
    }
    catch (const std::logic_error &e)
    {
        std::cout << "logic_error:" << std::endl;
        std::cout << e.what() << std::endl;
    }
    std::cout << "adr: '" << n1.GetAddressStr() << "', '" << n1.GetAddressInt() << "'" << std::endl;
    
    std::cout << "Setting address to 'asd', this should throw" << std::endl;
    try 
    {
        n1.SetAddress("asd");
    }
    catch (const std::logic_error &e)
    {
        std::cout << "logic_error:" << std::endl;
        std::cout << e.what() << std::endl;
    }
    std::cout << "adr: '" << n1.GetAddressStr() << "', '" << n1.GetAddressInt() << "'" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Str constructor:" << std::endl;
    std::cout << "Constructing with address '127.0.0.1', should pass" << std::endl;
    NetworkInterface n2 = NetworkInterface("127.0.0.1");
    std::cout << "adr: '" << n2.GetAddressStr() << "', '" << n2.GetAddressInt() << "'" << std::endl;
        
    std::cout << "Constructing with address '51289075', should fail to default" << std::endl;
    NetworkInterface n3 = NetworkInterface("51289075");
    std::cout << "adr: '" << n3.GetAddressStr() << "', '" << n3.GetAddressInt() << "'" << std::endl;
    
    std::cout << "Constructing with address 'asd', should fail to default" << std::endl;
    NetworkInterface n4 = NetworkInterface("asd");
    std::cout << "adr: '" << n4.GetAddressStr() << "', '" << n4.GetAddressInt() << "'" << std::endl;
    
    std::cout << "Constructing with address '255.255.255.255', should pass" << std::endl;
    NetworkInterface n5 = NetworkInterface("255.255.255.255");
    std::cout << "adr: '" << n5.GetAddressStr() << "', '" << n5.GetAddressInt() << "'" << std::endl;
    std::cout << std::endl;

    println("=== Testing Packet ===");
    std::cout << "Default constructor" << std::endl;
    Packet p1;
    std::cout << "data: " << p1.GetData() << std::endl;
    std::cout << "target: " << p1.GetTargetAddress() << std::endl;
    std::cout << "source: " << p1.GetSourceAddress() << std::endl;
    std::cout << "size: " << p1.GetSize() << std::endl;
    std::cout << "ttl: " << (int)p1.GetTimeToLive() << std::endl;
    std::cout << "Decremented ttl: " << (int)p1.DecrementTimeToLive() << std::endl;    
    std::cout << "Manual override size - 25: " << p1.SetSize(25) << std::endl;
    std::cout << "Manual override size - 0: " << p1.SetSize(0) << std::endl;
    std::cout << std::endl;

    std::cout << "Proper constructor" << std::endl;
    Packet p2 = Packet("Hello World!", 0xFF000000, 0xFF000001);
    std::cout << "data: " << p2.GetData() << std::endl;
    std::cout << "target: " << p2.GetTargetAddress() << std::endl;
    std::cout << "source: " << p2.GetSourceAddress() << std::endl;
    std::cout << "size: " << p2.GetSize() << std::endl;
    std::cout << "ttl: " << (int)p2.GetTimeToLive() << std::endl;
    std::cout << "Decremented ttl: " << (int)p2.DecrementTimeToLive() << std::endl;    
    std::cout << "Manual override size - 25: " << p2.SetSize(25) << std::endl;
    std::cout << "Manual override size - 0: " << p2.SetSize(0) << std::endl;
    


    return 0;
}