#include "test-modules.h"
#include <stdio.h>

int main() {
    int x = 0;
    scanf("%d", &x);
    // test(x, 2 + 7*x);
    printf("Facktorial of %d is %d\n", x, facktorial(x));

    return 0;
}