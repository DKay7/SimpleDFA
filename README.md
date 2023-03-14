# Logger compiler

## Description

This project implements simple logger compiler based on clang-13. 
For now, compiler just instruments every function and print to log file values (only integer ones) this function was called with in next format:

```csv
llvm_function_ptr_1,value_1,value_2, ...
llvm_function_ptr_2,value_1,value_2, ...
```

## How to build

To build this project, you need installed next requirements: `llvm-13 headers`, `clang-13`, `clang++-13`, `llvm-config-13`.

Follow next steps from project's root:
1. `mkdir build && cd build`
2. `cmake -DLLVM_DIR="/usr/lib/llvm-13/lib/cmake/llvm/" -DCMAKE_C_COMPILER=clang-13 -DCMAKE_CXX_COMPILER=clang++-13`. But replace `-DLLVM_DIR` value with path to your llvm's directory.
3. `make`

## How to run
you can run `./logger_cc` binary as usual clang compiler. For example, to compile given tests, you should execute (from projects build directory):
1. `./logger_cc ../tests/test.cpp ../tests/test-modules.cpp  -o test`
2. Then just run `./test` to collect runtime information. Then, `./build/instruction_values.log` file with collected info will appear.




