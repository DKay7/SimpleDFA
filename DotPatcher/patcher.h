#include <inttypes.h>
#include <map>
#include <string>
#include <vector>

using instr_map = std::map<int64_t, std::vector<int64_t>>;
instr_map read_runtime_values (std::string& log_file_path);
void patch_dot_file(std::string dot_file_path, instr_map& values_map);