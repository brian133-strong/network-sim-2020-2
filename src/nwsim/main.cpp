#include <iostream>
#include "./tests/testroutines.hpp"

//print line
void println(const std::string& s) {
    std::cout << s << std::endl;
}
int main(void) {
    PacketTestRoutine();
    AddressTestRoutine();
    NetworkInterfaceTestRoutine();
    NodeTestRoutine();
    EndHostTestRoutine();
    RouterTestRoutine();
    LinkTestRoutine();
    NetworkTestRoutine();
    return 0;
}
