
#include "patcher.h"
#include <string>
#include <iostream>

int main(int argc, const char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage:\n./patcher path_to_dot_file path_to_log_file\n"; 
        return -1;
    }

    std::string runtime_log_path{argv[1]};
    instr_map values_map = read_runtime_values(runtime_log_path);

    std::string dot_path{argv[2]};
    patch_dot_file(dot_path, values_map);

    return 0;
}