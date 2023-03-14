set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


add_library(dfa-pass MODULE DFA-pass.cpp)
set_target_properties(dfa-pass PROPERTIES PREFIX "")


add_executable(logger_cc logger_cc.c)


# a little bit of classic CMAKE cringe to copy object file near to executables 
add_library(instr.o OBJECT instr.cpp)
add_custom_command(TARGET OUTPUT "${CMAKE_BINARY_DIR}/instr.o"
                    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_OBJECTS:instr.o>" "${CMAKE_BINARY_DIR}/instr.o"
                    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/instr.cpp")
add_custom_target(instr ALL DEPENDS "${CMAKE_BINARY_DIR}/instr.o")


