# Logger compiler

## Description

This project implements simple logger compiler based on clang-13.

For now, compiler just instruments every instruction that returns _integer_ value and prints to the log file the values these instructions returned in next format:

```csv
llvm_function_ptr_1 value
llvm_function_ptr_2 value
...
```

Also, compiler dumped a def-use chain graph of every function in `dot` format.

Moreover, project contains patcher to insert collected runtime information to the def-use chain of the function.

## How to build

To build this project, you need installed next requirements: `llvm-13 headers`, `clang-13`, `clang++-13`, `llvm-config-13`.

Follow next steps from project's root:
1. `mkdir build && cd build`
2. `cmake -DLLVM_DIR="/usr/lib/llvm-13/lib/cmake/llvm/" -DCMAKE_C_COMPILER=clang-13 -DCMAKE_CXX_COMPILER=clang++-13`. But replace `-DLLVM_DIR` value with path to your llvm's directory.
3. `make`

## How to run
Running is consists of three main steps:
1. ### Static code instrumentation: 
    There's `logger_cc` to instrument code. You can run `./logger_cc` binary as usual clang compiler. For example, to compile given tests, you should execute (from projects build directory):

    `./logger_cc ../tests/test.cpp ../tests/test-modules.cpp  -o test`

    This will produce both: result binary file and *.dot files with def-use chain for each translation module.

2. ### Collecting runtime values information 
    To collect the data just run `./test` (or any other binary produced by compiler on step #1). Then, `./build/instruction_values.log` file with collected info will appear.

3. ### Combining all together
    Now, we need to unite runtime values information from step #2 with def-use chain graph from step #1. To do this, just run (from projects build directory):

    `./pathcher [path_to_produced_dot_file] [path_to_log_file_with_rt_info]`

    This script will insert rt values info to dot file. 
    
    *Note:* that script doesn't create new *.dot files, it replaces existing one. 
    
    Then, you can compile dot file like usual:

    `dot -Tpng [path_to_produced_dot_file] -o [path_to_create_output_file]`

    *Note:* if instruction was called several times (for ex. in recursive function), then instruction values will be listed in comma-separated list in graph.









