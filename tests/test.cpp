#include "test-modules.h"
#include <iostream>
#include <vector>

int main(void) {
    int x = 0;
    std::cin >> x;
    
    std::cout << x << " factorial:\n";
    std::cout << x << "! = " << faсtorial(x) << "\n";
    
    std::cout << "First " << x << " fibonnachi numbers:\n";
    print_fibonacchi(x);
    
    return 0;
}