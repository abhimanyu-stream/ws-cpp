# Topic 18 — C++ Tools and Build Systems

## Table of Contents
1. [Compilers](#1-compilers)
2. [Debuggers](#2-debuggers)
3. [Build Systems](#3-build-systems)
4. [Package Managers](#4-package-managers)
5. [Testing Frameworks](#5-testing-frameworks)
6. [Sanitizers](#6-sanitizers)
7. [Static Analysis](#7-static-analysis)
8. [Formatting](#8-clang-format)
9. [Profiling and Benchmarking](#9-profiling-and-benchmarking)
10. [Complete CMake Example](#10-complete-cmake-example)

---

## 1. Compilers

### GCC / G++
```bash
# Compile with C++17, all warnings
g++ -std=c++17 -Wall -Wextra -Wpedantic -o app main.cpp

# Debug build
g++ -std=c++17 -g -O0 -o app main.cpp

# Release build (optimized)
g++ -std=c++17 -O3 -DNDEBUG -o app main.cpp

# Generate assembly output
g++ -std=c++17 -S -O2 main.cpp -o main.s

# Show all predefined macros
g++ -std=c++17 -dM -E - < /dev/null
```

### Clang
```bash
# Same flags as GCC — clang is a drop-in replacement for most purposes
clang++ -std=c++17 -Wall -o app main.cpp

# Generate LLVM IR
clang++ -std=c++17 -S -emit-llvm main.cpp -o main.ll

# Show include tree
clang++ -std=c++17 -H main.cpp 2>&1 | head -20
```

### MSVC (Visual Studio)
```cmd
cl /std:c++17 /W4 /EHsc main.cpp /Fapp.exe
cl /std:c++17 /Od /Zi main.cpp    :: Debug
cl /std:c++17 /O2 main.cpp        :: Release
```

### Key Compiler Flags

| Flag | GCC/Clang | MSVC | Meaning |
|---|---|---|---|
| C++ standard | `-std=c++17` | `/std:c++17` | Use C++17 |
| All warnings | `-Wall -Wextra` | `/W4` | Enable warnings |
| Warnings as errors | `-Werror` | `/WX` | Treat warnings as errors |
| Debug info | `-g` | `/Zi` | Include debug symbols |
| Optimization | `-O2` / `-O3` | `/O2` | Optimize |
| No optimization | `-O0` | `/Od` | For debugging |
| Define macro | `-DNAME=val` | `/DNAME=val` | Preprocessor define |
| Exception handling | (default on) | `/EHsc` | Enable exceptions |

---

## 2. Debuggers

### GDB (GNU Debugger)
```bash
# Compile with debug info
g++ -std=c++17 -g -O0 -o app main.cpp

# Start GDB
gdb ./app

# GDB commands:
# run [args]       — start the program
# break main       — breakpoint at function
# break file.cpp:42 — breakpoint at line 42
# next (n)         — step over
# step (s)         — step into
# continue (c)     — continue until next breakpoint
# print var        — print variable value
# info locals      — show all local variables
# backtrace (bt)   — show call stack
# watch var        — stop when var changes
# quit (q)         — exit GDB
```

### LLDB (Clang's Debugger)
```bash
lldb ./app

# LLDB commands:
# run / r          — start the program
# breakpoint set --name main
# breakpoint set --file main.cpp --line 42
# next / n         — step over
# step / s         — step into
# continue / c     — continue
# frame variable   — show local variables
# bt               — backtrace
```

### VS Code Debugging (with Cygwin64 GCC)
```json
// .vscode/launch.json
{
    "version": "0.2.0",
    "configurations": [{
        "name": "Debug C++",
        "type": "cppdbg",
        "request": "launch",
        "program": "${workspaceFolder}/build/${fileBasenameNoExtension}",
        "args": [],
        "stopAtEntry": false,
        "cwd": "${workspaceFolder}",
        "MIMode": "gdb",
        "miDebuggerPath": "E:/Installation/CTools/cygwin64/bin/gdb.exe"
    }]
}
```

---

## 3. Build Systems

### CMake
```bash
# Configure
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build

# Build specific target
cmake --build build --target my_app

# Install
cmake --install build --prefix /usr/local

# Delete cache and reconfigure
rm build/CMakeCache.txt
cmake -S . -B build
```

#### CMakeLists.txt quick reference
```cmake
cmake_minimum_required(VERSION 3.16)
project(my_project VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # for clangd

# Executable
add_executable(my_app main.cpp utils.cpp)
target_include_directories(my_app PRIVATE include/)
target_compile_options(my_app PRIVATE -Wall -Wextra)

# Static library
add_library(mylib STATIC lib.cpp)
target_include_directories(mylib PUBLIC include/)

# Link
target_link_libraries(my_app PRIVATE mylib)

# FetchContent (download dependencies)
include(FetchContent)
FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip)
FetchContent_MakeAvailable(googletest)
```

#### CMakePresets.json
```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "debug",
      "binaryDir": "${sourceDir}/build/debug",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug",
        "CMAKE_CXX_COMPILER": "E:/Installation/CTools/cygwin64/bin/g++.exe",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
      }
    },
    {
      "name": "release",
      "binaryDir": "${sourceDir}/build/release",
      "cacheVariables": { "CMAKE_BUILD_TYPE": "Release" }
    }
  ],
  "buildPresets": [
    { "name": "debug",   "configurePreset": "debug"   },
    { "name": "release", "configurePreset": "release" }
  ]
}
```

```bash
cmake --preset debug
cmake --build --preset debug
```

### Ninja
```bash
# Ninja is a fast build backend used by CMake
cmake -S . -B build -G Ninja
cmake --build build        # internally runs ninja

# Or run ninja directly
cd build && ninja
ninja -j8                  # parallel (8 jobs)
ninja clean
```

### Make
```bash
# CMake can generate Makefiles too
cmake -S . -B build -G "Unix Makefiles"
make -C build -j4
make -C build clean
```

---

## 4. Package Managers

### vcpkg (Microsoft)
```bash
# Install vcpkg
git clone https://github.com/microsoft/vcpkg
./vcpkg/bootstrap-vcpkg.sh

# Install packages
./vcpkg install nlohmann-json
./vcpkg install boost
./vcpkg install gtest

# Integrate with CMake
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### Conan
```bash
pip install conan

# conanfile.txt
[requires]
nlohmann_json/3.11.3
gtest/1.14.0

[generators]
cmake

# Install dependencies
conan install . --output-folder=build --build=missing
```

---

## 5. Testing Frameworks

### Google Test
```cmake
# In CMakeLists.txt
include(FetchContent)
FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(my_tests test_main.cpp)
target_link_libraries(my_tests GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(my_tests)
```

```cpp
// test_main.cpp
#include <gtest/gtest.h>

int add(int a, int b) { return a + b; }

TEST(AddTest, PositiveNumbers) {
    EXPECT_EQ(add(2, 3), 5);
    EXPECT_EQ(add(0, 0), 0);
}

TEST(AddTest, NegativeNumbers) {
    EXPECT_EQ(add(-1, -2), -3);
    ASSERT_NE(add(1, 1), 3);    // ASSERT stops test on failure, EXPECT continues
}

// Test fixture
class MyFixture : public ::testing::Test {
protected:
    void SetUp() override    { /* runs before each test */ }
    void TearDown() override { /* runs after each test */ }
    std::vector<int> data = {1,2,3,4,5};
};

TEST_F(MyFixture, SumTest) {
    int sum = 0;
    for (auto x : data) sum += x;
    EXPECT_EQ(sum, 15);
}
```

```bash
# Run tests
ctest --test-dir build -V        # verbose
ctest --test-dir build -R "Add*" # filter by name
./my_tests --gtest_filter="AddTest*"
```

### Catch2
```cpp
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("add works", "[math]") {
    REQUIRE(add(2,3) == 5);
    CHECK(add(0,0) == 0);
}

SECTION / GIVEN / WHEN / THEN — BDD style supported too
```

---

## 6. Sanitizers

Sanitizers are compiler instrumentation that catch bugs at runtime.

### AddressSanitizer (ASan) — detects memory errors
```bash
g++ -std=c++17 -fsanitize=address -g -o app main.cpp
./app

# Detects: heap/stack buffer overflow, use-after-free, memory leaks, double-free
```

### UndefinedBehaviorSanitizer (UBSan)
```bash
g++ -std=c++17 -fsanitize=undefined -g -o app main.cpp

# Detects: signed overflow, null dereference, bad casts, out-of-bounds, alignment
```

### ThreadSanitizer (TSan) — detects data races
```bash
g++ -std=c++17 -fsanitize=thread -g -o app main.cpp

# NOTE: cannot use with ASan simultaneously
```

### Combine sanitizers
```bash
g++ -std=c++17 -fsanitize=address,undefined -g -O1 -o app main.cpp
```

### In CMake
```cmake
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
if(ENABLE_ASAN)
    target_compile_options(my_app PRIVATE -fsanitize=address,undefined -g)
    target_link_options(my_app    PRIVATE -fsanitize=address,undefined)
endif()
```
```bash
cmake -S . -B build -DENABLE_ASAN=ON
```

---

## 7. Static Analysis

### Clang-Tidy
```bash
# Run on a file
clang-tidy main.cpp -- -std=c++17

# With compile_commands.json (generated by CMake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
clang-tidy main.cpp

# Common checks
clang-tidy main.cpp -checks=modernize-*,readability-*,performance-*,bugprone-*
```

**.clang-tidy** config file:
```yaml
Checks: >
  modernize-use-auto,
  modernize-use-nullptr,
  modernize-use-override,
  readability-braces-around-statements,
  performance-unnecessary-copy-initialization,
  bugprone-use-after-move

WarningsAsErrors: '*'
```

### Cppcheck
```bash
cppcheck --enable=all --std=c++17 src/
cppcheck --enable=all --suppress=missingIncludeSystem src/
```

---

## 8. Clang-Format

Auto-formats C++ code to a consistent style.

```bash
# Format a file in-place
clang-format -i main.cpp

# Format all cpp/h files
find . -name "*.cpp" -o -name "*.h" | xargs clang-format -i

# Check (don't modify, exit non-zero if changes needed — for CI)
clang-format --dry-run --Werror main.cpp
```

**.clang-format** config file (place in project root):
```yaml
BasedOnStyle: Google
IndentWidth: 4
ColumnLimit: 100
AllowShortFunctionsOnASingleLine: Inline
BreakBeforeBraces: Allman
SortIncludes: true
```

---

## 9. Profiling and Benchmarking

### gprof (GCC profiler)
```bash
g++ -pg -O2 -o app main.cpp    # compile with profiling
./app                           # run to generate gmon.out
gprof app gmon.out | head -30  # view profile
```

### Valgrind (memory analysis + profiling)
```bash
# Memory check
valgrind --leak-check=full ./app

# Cache profiling
valgrind --tool=callgrind ./app
callgrind_annotate callgrind.out.*
```

### perf (Linux kernel profiler)
```bash
perf stat ./app              # basic stats (CPU cycles, cache misses, etc.)
perf record ./app            # record profile
perf report                  # view results
```

### Google Benchmark (micro-benchmarking)
```cpp
#include <benchmark/benchmark.h>

static void BM_StringCopy(benchmark::State& state) {
    std::string s = "Hello, World!";
    for (auto _ : state) {
        std::string copy = s;         // what we're benchmarking
        benchmark::DoNotOptimize(copy);
    }
}
BENCHMARK(BM_StringCopy);
BENCHMARK_MAIN();
```
```bash
./bench --benchmark_filter=BM_StringCopy
```

### Quick manual benchmark
```cpp
#include <chrono>

auto t1 = std::chrono::high_resolution_clock::now();
// ... code to measure ...
auto t2 = std::chrono::high_resolution_clock::now();
auto us = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
std::cout << "Elapsed: " << us << " μs\n";
```

---

## 10. Complete CMake Example

A production-ready project structure using the tools above:

```
my_project/
├── CMakeLists.txt
├── CMakePresets.json
├── .clang-tidy
├── .clang-format
├── .gitignore
├── src/
│   ├── CMakeLists.txt
│   └── main.cpp
├── lib/
│   ├── CMakeLists.txt
│   ├── mylib.cpp
│   └── include/mylib/mylib.h
└── tests/
    ├── CMakeLists.txt
    └── test_mylib.cpp
```

**Root CMakeLists.txt:**
```cmake
cmake_minimum_required(VERSION 3.20)
project(my_project VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS  "Build tests"       ON)
option(ENABLE_ASAN  "AddressSanitizer"  OFF)

if(ENABLE_ASAN)
    add_compile_options(-fsanitize=address,undefined -g)
    add_link_options(   -fsanitize=address,undefined)
endif()

add_subdirectory(lib)
add_subdirectory(src)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
```

**Build commands:**
```bash
# Debug with sanitizers
cmake --preset debug -DENABLE_ASAN=ON
cmake --build --preset debug

# Run tests
ctest --test-dir build/debug -V

# Release
cmake --preset release
cmake --build --preset release

# Lint
clang-tidy src/*.cpp

# Format
clang-format -i src/*.cpp lib/**/*.cpp
```
