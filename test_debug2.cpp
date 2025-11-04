#include "linked_hashmap.hpp"
#include <iostream>

int main() {
    sjtu::linked_hashmap<int, int> map;
    
    // Test with empty map
    std::cout << "Empty map size: " << map.size() << std::endl;
    
    // This might cause issues
    auto it = map.end();
    if (it != map.begin()) {
        --it;
        std::cout << "After decrement: " << it->first << std::endl;
    } else {
        std::cout << "Map is empty, cannot decrement end()" << std::endl;
    }
    
    return 0;
}

