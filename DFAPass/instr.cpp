#include <inttypes.h>
#include <fstream>
#include <iostream>

std::ofstream log_file;

extern "C" void open_log() {
    log_file.open("instruction_values.log");
}

extern "C" void instruction_logger(int64_t instr_ptr, int64_t instr_value) {
    log_file << instr_ptr << "\t\t" << instr_value << "\n";    
}

