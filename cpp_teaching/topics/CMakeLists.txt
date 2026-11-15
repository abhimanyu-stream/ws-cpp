cmake_minimum_required(VERSION 3.16)
project(cpp_teaching_topics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

# Each topic is an independent executable
set(TOPICS
    01_fundamentals/01_fundamentals.cpp
    02_functions/02_functions.cpp
    03_pointers/03_pointers.cpp
    04_function_pointers/04_function_pointers.cpp
    05_references/05_references.cpp
    06_arrays_and_strings/06_arrays_strings.cpp
    07_oop/07_oop.cpp
    08_templates/08_templates.cpp
    09_stl/09_stl.cpp
    10_memory_smart_pointers/10_memory_smart_pointers.cpp
    11_exception_handling/11_exception_handling.cpp
    12_modern_cpp/12_modern_cpp.cpp
    15_advanced_cpp/15_advanced_cpp.cpp
    16_design_patterns/16_design_patterns.cpp
    17_data_structures_algorithms/17_dsa.cpp
)

foreach(SRC ${TOPICS})
    # Extract folder name as target name
    get_filename_component(DIR  ${SRC} DIRECTORY)
    get_filename_component(NAME ${SRC} NAME_WE)
    add_executable(topic_${NAME} ${SRC})
endforeach()

# Topic 13 needs pthread
add_executable(topic_13_multithreading 13_multithreading/13_multithreading.cpp)
target_link_libraries(topic_13_multithreading PRIVATE
    $<$<NOT:$<PLATFORM_ID:Windows>>:pthread>
)

# Topic 14 needs filesystem
add_executable(topic_14_file_handling 14_file_handling/14_file_handling.cpp)
target_link_libraries(topic_14_file_handling PRIVATE
    $<$<AND:$<NOT:$<PLATFORM_ID:Windows>>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>
)
