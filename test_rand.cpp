#include <iostream>
const int MOD = 998244353;
int cur = 233, base = 2333;
inline int rand(){
    return 1ll*cur*base%MOD;
}

int main() {
    for (int i = 0; i < 10; i++) {
        std::cout << rand() << std::endl;
    }
    return 0;
}

