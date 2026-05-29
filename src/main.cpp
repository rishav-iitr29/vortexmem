#include <iostream>
#include "Config.hpp"

int main() {
    std::cout << "--- VortexMem Simulator Booting ---" << std::endl;
    std::cout << "Default L1 Hit Time: " << config::HIT_TIME_L1 << " cycle(s)." << std::endl;
    return 0;
}