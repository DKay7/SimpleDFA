#include <inttypes.h>
#include <fstream>


std::ofstream log_file;

extern "C" void open_log() {
    log_file.open("instruction_values.log");
}

extern "C" void function_logger(int64_t instr_ptr, int64_t num_args, ...) {

    va_list args;
    va_start(args, num_args);
    printf("LOGGER WAS CALLED WITH %d ARGS\n", num_args);
    log_file << instr_ptr;
    for(int idx = 0; idx != num_args; ++idx) {
        int next_arg = va_arg(args, int);
        log_file << "," << next_arg;
    }
    log_file << "\n";
    
    va_end(args);
}

extern "C" void close_log() {
    log_file.close();
}
