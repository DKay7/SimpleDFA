#include <iostream>

int faсtorial(int x) {
    if (x <= 1)
        return 1;
    else 
        return faсtorial(x - 1) * x;
}

void print_fibonacchi(int x) {
    int prev_num = 1, cur_num = 1;

    std::cout << cur_num;

    for (int idx = 1; idx < x; ++idx) {
        std::cout << ", " << cur_num;
        int tmp = prev_num;
        prev_num = cur_num;
        cur_num += tmp;
    }    

    std::cout << "\n";

}