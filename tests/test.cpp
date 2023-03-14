#include "test-modules.h"
#include <stdio.h>

int main() {
    int x = 0;
    scanf("%d", &x);
    test(x, 2 + 7*x);
    return 0;
}