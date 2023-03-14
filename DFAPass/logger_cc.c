#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <inttypes.h>

int main(int argc, char* argv[]) {
    uint8_t** cc_params = (uint8_t**) calloc(argc + 10, sizeof(*cc_params));

    for (int idx = 1; idx != argc; ++idx) {
        cc_params[idx] = argv[idx];
    }

    cc_params[argc++] = "-fpass-plugin=./dfa-pass.so";
    cc_params[argc++] = "instr.o";
    cc_params[0] = "clang++-13";

    execvp(cc_params[0], (char **)cc_params);

    free(cc_params);
    return 0;
} 