#include "linked_hashmap.hpp"
#include <iostream>

int main() {
    sjtu::linked_hashmap<int, int> map;
    
    // Insert some elements
    map[1] = 10;
    map[2] = 20;
    map[3] = 30;
    
    std::cout << "Size: " << map.size() << std::endl;
    
    // Test forward iteration
    std::cout << "Forward iteration:" << std::endl;
    for (auto it = map.begin(); it != map.end(); ++it) {
        std::cout << it->first << " -> " << it->second << std::endl;
    }
    
    // Test backward iteration
    std::cout << "Backward iteration:" << std::endl;
    auto it = map.end();
    --it;  // This should work
    std::cout << "After first decrement: " << it->first << " -> " << it->second << std::endl;
    
    for (it = --map.end(); it != map.begin(); --it) {
        std::cout << it->first << " -> " << it->second << std::endl;
    }
    std::cout << it->first << " -> " << it->second << std::endl;
    
    return 0;
}

