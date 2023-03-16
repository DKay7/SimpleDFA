#include <stdio.h>

// void test(int x, int y) {
//     printf("%d\n%d\n", x, y);
// }

// void another_test(int* x, int *y){
//     (*x)++;
//     (*y)++;
// }

int facktorial(int x) {
    if (x <= 1)
        return 1;
    else 
        return facktorial(x - 1) * x;
}