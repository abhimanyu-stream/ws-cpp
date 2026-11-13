set(LESSONS
    01_stack_vs_heap
    02_pointers_and_references
    03_smart_pointers
    04_copy_vs_move
    05_memory_leaks_dangling
    06_virtual_polymorphism
    07_stl_containers
    08_stl_algorithms
    09_stl_iterators
    10_stl_functors_lambdas
    11_multithreading
    12_exception_safety
    13_raii
)

foreach(LESSON ${LESSONS})
    add_executable(${LESSON} ${LESSON}.cpp)
    target_include_directories(${LESSON} PRIVATE ${CMAKE_SOURCE_DIR}/include)
endforeach()
