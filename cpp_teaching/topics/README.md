# C++ Teaching Topics — Complete Program Index

Each folder contains a self-contained `.cpp` file with inline notes explaining every concept.
Compile and run each file independently to see the output.

---

## How to Compile Each Topic

### Using Cygwin64 GCC (this workspace setup)
```bash
g++ -std=c++17 -Wall -Wextra topics/01_fundamentals/01_fundamentals.cpp -o build/t01
g++ -std=c++17 -Wall -Wextra topics/02_functions/02_functions.cpp        -o build/t02
# ... and so on
```

### Using CMake (builds all topics at once)
```bash
cmake -S topics/ -B build/topics \
      -DCMAKE_CXX_COMPILER="E:/Installation/CTools/cygwin64/bin/g++.exe" \
      -G Ninja
cmake --build build/topics
```

---

## Topic Index

| # | Folder | File | Key Concepts |
|---|--------|------|-------------|
| 01 | `01_fundamentals` | `01_fundamentals.cpp` | Variables, types, operators, control flow, namespaces, `auto`, `const`, `constexpr`, scope |
| 02 | `02_functions` | `02_functions.cpp` | Pass by value/ref/ptr, default args, overloading, inline, recursion, lambdas, variadic, trailing return |
| 03 | `03_pointers` | `03_pointers.cpp` | Declaration, arithmetic, null/void/wild/dangling, `int**`, const variants, arrays, strings, dynamic memory, shallow vs deep copy |
| 04 | `04_function_pointers` | `04_function_pointers.cpp` | Syntax, as argument/return, dispatch tables, member fn ptrs, callbacks, vs lambda/`std::function`/`std::bind` |
| 05 | `05_references` | `05_references.cpp` | lvalue ref, rvalue ref, `const T&`, ref-to-ptr, ref-to-array, returning refs, `std::move`, `std::forward`, collapsing |
| 06 | `06_arrays_and_strings` | `06_arrays_strings.cpp` | 1D/2D/3D arrays, decay, `std::array`, `std::span`, C-strings, `std::string`, `std::string_view`, raw literals |
| 07 | `07_oop` | `07_oop.cpp` | Classes, access specifiers, all constructors, Rule of 3/5/0, inheritance, polymorphism, vtable, abstract, operator overloading, diamond problem |
| 08 | `08_templates` | `08_templates.cpp` | Function/class templates, all parameter types, specialization, SFINAE, type traits, variadic, fold, `if constexpr`, CRTP, Concepts (C++20) |
| 09 | `09_stl` | `09_stl.cpp` | All containers, all iterator types, 20+ algorithms, `pair`, `tuple`, `optional`, `variant`, `any`, `std::function`, `bind` |
| 10 | `10_memory_smart_pointers` | `10_memory_smart_pointers.cpp` | Stack/heap/static, RAII, `unique_ptr`, `shared_ptr`, `weak_ptr`, custom deleters, circular refs, ownership semantics |
| 11 | `11_exception_handling` | `11_exception_handling.cpp` | `try/catch/throw`, multiple catch, custom exceptions, propagation, stack unwind, `noexcept`, safety guarantees, standard hierarchy |
| 12 | `12_modern_cpp` | `12_modern_cpp.cpp` | C++11/14/17/20/23 features side-by-side: lambdas, move, structured bindings, `optional`, `variant`, ranges, concepts, `std::expected` |
| 13 | `13_multithreading` | `13_multithreading.cpp` | `std::thread`, mutex, `lock_guard`, `condition_variable`, `atomic`, memory ordering, futures, promises, `async`, producer-consumer, deadlock |
| 14 | `14_file_handling` | `14_file_handling.cpp` | `ifstream`, `ofstream`, `fstream`, binary I/O, file positioning, CSV serialization, `stringstream`, `std::filesystem` |
| 15 | `15_advanced_cpp` | `15_advanced_cpp.cpp` | UB, storage duration, linkage, name mangling, vtable/vptr, RTTI, 4 casts, placement new, cache locality, alignment/padding |
| 16 | `16_design_patterns` | `16_design_patterns.cpp` | Singleton, Factory, Abstract Factory, Builder, Prototype, Adapter, Decorator, Proxy, Facade, Observer, Strategy, Command, State, DI |
| 17 | `17_data_structures_algorithms` | `17_dsa.cpp` | Linked list, BST, Graph BFS/DFS, all sorting, binary search, DP (knapsack/LCS/fib), two pointers, sliding window, bit manipulation |
| 18 | `18_tools_build_systems` | `18_tools_build_systems.md` | GCC/Clang/MSVC, GDB/LLDB, CMake, Ninja, Make, vcpkg, Conan, GoogleTest, Catch2, ASan/TSan/UBSan, clang-tidy, clang-format, profiling |

---

## Learning Path (from the checklist)

```
Step 1  → Topics 01, 02, 06        Fundamentals, Functions, Arrays
Step 2  → Topics 03, 05, 10        Pointers, References, Memory
Step 3  → Topic  04                Function Pointers & Callbacks
Step 4  → Topic  07                OOP & Polymorphism
Step 5  → Topic  08                Templates & Metaprogramming
Step 6  → Topic  09                STL Containers, Iterators, Algorithms
Step 7  → Topic  10, 11            Smart Pointers, RAII, Exceptions
Step 8  → Topic  12                Modern C++11–C++23
Step 9  → Topic  13                Multithreading & Concurrency
Step 10 → Topics 15, 14, 16, 17   Advanced C++, Files, Patterns, DSA
```

---

## Companion Reference Files

| File | Contents |
|------|----------|
| `LVALUE_RVALUE.md` | Deep dive: lvalue vs rvalue, move semantics, RVO |
| `CONST_GUIDE.md` | All `const` variants, how to read declarations, `constexpr` vs `const` |
| `REFERENCES_T_REF_VS_TREF_REF.md` | `T&` vs `T&&`, universal refs, `std::forward`, collapsing rules |
| `CMAKE_GUIDE.md` | CMake from basic to advanced with usage examples |
| `src/00_cpp17_syntax_guide.cpp` | All C++17 syntax in one runnable file |
