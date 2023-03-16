#include <map>
#include <inttypes.h>
#include <fstream>
#include <sstream>
#include <regex>
#include <iostream> // TODO remove
#include <concepts>
#include <functional>

#include "patcher.h"

instr_map read_runtime_values (std::string& log_file_path) {

    instr_map values_map;
    std::ifstream log_file(log_file_path);
    int64_t instruction_ptr, instruction_value;

    while (log_file >> instruction_ptr >> instruction_value) {
        auto existing_value = values_map.find(instruction_ptr);
        if (existing_value != values_map.end())
            values_map[instruction_ptr].push_back(instruction_value);
        else
            values_map[instruction_ptr] = {instruction_value};
    
    }

    return values_map;
}

std::string read_whole_file(const std::string &file_path) {
    std::ifstream dot_file(file_path);

    std::stringstream dot_file_buffer;
    dot_file_buffer << dot_file.rdbuf();

    return dot_file_buffer.str();
}

std::string regex_replace(std::string target, std::string replacement, std::invocable<const std::smatch&> auto&& replacer) {
    std::regex replacement_regex(replacement);

    std::string replaced_string;
    int previous_position_index = 0;

    auto iter = std::sregex_iterator(target.begin(), target.end(), replacement_regex);
    for (; iter != std::sregex_iterator(); ++ iter) {
        int current_position_index = iter->position();
        int delta_size = current_position_index - previous_position_index;

        replaced_string += target.substr(previous_position_index, delta_size);
        replaced_string += std::invoke(replacer, *iter);

        previous_position_index = current_position_index + iter->length();
    }

    return replaced_string += target.substr(previous_position_index);
}

void patch_dot_file(std::string dot_file_path, instr_map& values_map) {
    std::string dot_file_str = read_whole_file(dot_file_path);

    std::string tmp_dot_file_path = dot_file_path + "tmp~";
    std::ofstream dot_tmp_file(tmp_dot_file_path);

    auto match_callback = [&](auto &&match) {
        int64_t value_ptr = std::stoi(match[1]);
        auto value = values_map.find(value_ptr);
        if (value != values_map.end()) {
            std::stringstream values_str;
            for (auto& next_value : value->second) {
                values_str << next_value << ", ";
            }

            return "VALUE: " + values_str.str();
        }

        return std::string("??");
    };

    std::string patched_dot_str = regex_replace(dot_file_str, "VALUE=~~(\\d+)", match_callback); 
                                                

    dot_tmp_file << patched_dot_str;
    
    std::remove(dot_file_path.c_str());
    std::rename(tmp_dot_file_path.c_str(), dot_file_path.c_str());

    return;
}