#include <iostream>
#include "address.hpp"

//print line
void println(const std::string& s) {
    std::cout << s << std::endl;
}

int main(void) {
    // Test address
    println("=== TEST BLOCK: Address ===");
    Address adr_empty = Address();
    println("Empty constructor checks: ");
    std::cout << "GetAddressStr: " << adr_empty.GetAddressStr() << std::endl;
    std::cout << "GetAddressInt: " << adr_empty.GetAddressInt() << std::endl;

    return 0;
}