# CMake Guide — Basic to Advanced

## Table of Contents
1. [What is CMake?](#what-is-cmake)
2. [Basic Commands](#basic-commands)
3. [Project Structure](#project-structure)
4. [Targets & Libraries](#targets--libraries)
5. [Compiler Settings](#compiler-settings)
6. [Variables](#variables)
7. [Control Flow](#control-flow)
8. [Finding Packages](#finding-packages)
9. [FetchContent](#fetchcontent)
10. [Install Rules](#install-rules)
11. [Testing with CTest](#testing-with-ctest)
12. [Generator Expressions](#generator-expressions)
13. [Toolchain & Cross Compilation](#toolchain--cross-compilation)
14. [Advanced Patterns](#advanced-patterns)

---

## What is CMake?

CMake is a **build system generator** — it doesn't build your code directly. It generates build files (Makefiles, Ninja files, VS project files, etc.) from `CMakeLists.txt` files, then your build tool does the actual compilation.

```
CMakeLists.txt  →  cmake  →  Ninja / Make / MSBuild  →  binary
```

---

## Basic Commands

### Configure & Build

```bash
# Configure: generate build files into the build/ directory
cmake -S . -B build

# Build
cmake --build build

# Build with specific target
cmake --build build --target my_app

# Clean build
cmake --build build --target clean

# Parallel build (4 jobs)
cmake --build build -j 4
```

### Common CLI Flags

```bash
# Set a CMake variable from command line
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Specify generator
cmake -S . -B build -G Ninja
cmake -S . -B build -G "Unix Makefiles"

# Specify compiler
cmake -S . -B build -DCMAKE_CXX_COMPILER=/usr/bin/g++
cmake -S . -B build -DCMAKE_C_COMPILER=/usr/bin/gcc

# List available generators
cmake --help
```

### Build Types

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug      # -g, no optimization
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release     # -O3, no debug info
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo  # -O2 + -g
cmake -S . -B build -DCMAKE_BUILD_TYPE=MinSizeRel  # -Os
```

---

## Project Structure

### Minimal CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_project VERSION 1.0.0 LANGUAGES CXX)

add_executable(my_app main.cpp)
```

### Typical Multi-Directory Layout

```
my_project/
├── CMakeLists.txt          # root
├── src/
│   ├── CMakeLists.txt
│   └── main.cpp
├── lib/
│   ├── CMakeLists.txt
│   ├── mylib.cpp
│   └── mylib.h
└── tests/
    ├── CMakeLists.txt
    └── test_mylib.cpp
```

**Root CMakeLists.txt**
```cmake
cmake_minimum_required(VERSION 3.16)
project(my_project VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(tests)
```

**lib/CMakeLists.txt**
```cmake
add_library(mylib mylib.cpp)

target_include_directories(mylib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
```

**src/CMakeLists.txt**
```cmake
add_executable(my_app main.cpp)

target_link_libraries(my_app PRIVATE mylib)
```

---

## Targets & Libraries

### Executable

```cmake
add_executable(my_app main.cpp utils.cpp helper.cpp)
```

### Static Library (`.a` / `.lib`)

```cmake
add_library(mylib STATIC mylib.cpp)
```

### Shared Library (`.so` / `.dll`)

```cmake
add_library(mylib SHARED mylib.cpp)
```

### Header-Only (Interface) Library

```cmake
add_library(mylib INTERFACE)
target_include_directories(mylib INTERFACE include/)
```

### Object Library (compiled once, shared between targets)

```cmake
add_library(mylib_obj OBJECT mylib.cpp)

add_executable(app1 main1.cpp)
target_link_libraries(app1 PRIVATE mylib_obj)

add_executable(app2 main2.cpp)
target_link_libraries(app2 PRIVATE mylib_obj)
```

### Linking

```cmake
# PRIVATE  — used only internally by this target
# PUBLIC   — used internally AND propagated to dependents
# INTERFACE— not used internally, only propagated to dependents

target_link_libraries(my_app
    PRIVATE   mylib
    PUBLIC    another_lib
    INTERFACE header_only_lib
)
```

### Include Directories

```cmake
target_include_directories(my_app
    PRIVATE   src/
    PUBLIC    include/
    INTERFACE api/
)
```

### Compile Definitions

```cmake
target_compile_definitions(my_app
    PRIVATE   APP_VERSION="1.0"
    PUBLIC    MYLIB_EXPORTS
)
```

---

## Compiler Settings

### C++ Standard

```cmake
set(CMAKE_CXX_STANDARD 17)          # or 11, 14, 20, 23
set(CMAKE_CXX_STANDARD_REQUIRED ON) # error if standard not supported
set(CMAKE_CXX_EXTENSIONS OFF)       # use -std=c++17 not -std=gnu++17
```

### Per-Target Standard

```cmake
set_target_properties(my_app PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
```

### Compiler Warnings

```cmake
if(MSVC)
    target_compile_options(my_app PRIVATE /W4 /WX)
else()
    target_compile_options(my_app PRIVATE
        -Wall -Wextra -Wpedantic
        -Wshadow -Wconversion
        -Werror   # treat warnings as errors
    )
endif()
```

### Sanitizers (Debug builds)

```cmake
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(my_app PRIVATE -fsanitize=address,undefined)
    target_link_options(my_app    PRIVATE -fsanitize=address,undefined)
endif()
```

---

## Variables

### Setting Variables

```cmake
set(MY_VAR "hello")
set(MY_LIST a b c d)          # list variable
set(MY_VAR "hello" CACHE STRING "Description")  # cache variable (visible in cmake-gui)
```

### Reading Variables

```cmake
message(STATUS "Value: ${MY_VAR}")
message(STATUS "Source dir: ${CMAKE_SOURCE_DIR}")
message(STATUS "Build dir:  ${CMAKE_BINARY_DIR}")
```

### Useful Built-in Variables

| Variable | Meaning |
|---|---|
| `CMAKE_SOURCE_DIR` | Root source directory |
| `CMAKE_BINARY_DIR` | Root build directory |
| `CMAKE_CURRENT_SOURCE_DIR` | Current CMakeLists.txt directory |
| `CMAKE_CURRENT_BINARY_DIR` | Current build subdirectory |
| `CMAKE_CXX_COMPILER` | Full path to C++ compiler |
| `CMAKE_BUILD_TYPE` | Debug / Release / etc. |
| `CMAKE_INSTALL_PREFIX` | Install root (default `/usr/local`) |
| `PROJECT_NAME` | Name from `project()` |
| `PROJECT_VERSION` | Version from `project()` |

### Options (Boolean Cache Variables)

```cmake
option(ENABLE_TESTS "Build unit tests" ON)

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()
```

```bash
# Disable from command line
cmake -S . -B build -DENABLE_TESTS=OFF
```

---

## Control Flow

### if / elseif / else

```cmake
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Building on Linux")
elseif(WIN32)
    message(STATUS "Building on Windows")
else()
    message(STATUS "Unknown platform")
endif()
```

### Loops

```cmake
# foreach over a list
foreach(src main.cpp utils.cpp helper.cpp)
    message(STATUS "Source file: ${src}")
endforeach()

# foreach with range
foreach(i RANGE 1 5)
    message(STATUS "i = ${i}")
endforeach()

# while
set(counter 0)
while(counter LESS 3)
    message(STATUS "counter = ${counter}")
    math(EXPR counter "${counter} + 1")
endwhile()
```

### Functions and Macros

```cmake
# Function — has its own scope
function(add_my_executable name)
    add_executable(${name} ${ARGN})
    target_compile_options(${name} PRIVATE -Wall -Wextra)
    set_target_properties(${name} PROPERTIES CXX_STANDARD 17)
endfunction()

add_my_executable(app1 main1.cpp)
add_my_executable(app2 main2.cpp)

# Macro — shares caller's scope (like a text substitution)
macro(print_var var)
    message(STATUS "${var} = ${${var}}")
endmacro()

set(MY_PATH "/usr/local")
print_var(MY_PATH)   # prints: MY_PATH = /usr/local
```

---

## Finding Packages

### find_package

```cmake
# MODULE mode — looks for FindXxx.cmake in CMake's modules
find_package(OpenGL REQUIRED)

# CONFIG mode — looks for XxxConfig.cmake installed by the package
find_package(Boost 1.75 REQUIRED COMPONENTS filesystem system)

# Optional package
find_package(ZLIB)
if(ZLIB_FOUND)
    target_link_libraries(my_app PRIVATE ZLIB::ZLIB)
endif()
```

### Using Found Packages

```cmake
# Modern target-based (preferred)
target_link_libraries(my_app PRIVATE OpenGL::GL)

# Legacy variable-based (older packages)
target_include_directories(my_app PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(my_app PRIVATE ${Boost_LIBRARIES})
```

### pkg-config (Linux)

```cmake
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBPNG REQUIRED libpng)

target_include_directories(my_app PRIVATE ${LIBPNG_INCLUDE_DIRS})
target_link_libraries(my_app PRIVATE ${LIBPNG_LIBRARIES})
```

---

## FetchContent

Download and build dependencies at configure time — no manual install needed.

```cmake
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)

# Download and make available
FetchContent_MakeAvailable(googletest nlohmann_json)

# Now use them like any other target
target_link_libraries(my_tests PRIVATE GTest::gtest_main)
target_link_libraries(my_app   PRIVATE nlohmann_json::nlohmann_json)
```

### Offline / Local Override

```cmake
# Point to a local copy instead of downloading
set(FETCHCONTENT_SOURCE_DIR_GOOGLETEST "E:/libs/googletest" CACHE PATH "")
```

---

## Install Rules

```cmake
# Install executable
install(TARGETS my_app
    RUNTIME DESTINATION bin        # .exe on Windows
)

# Install library
install(TARGETS mylib
    ARCHIVE DESTINATION lib        # .a / .lib
    LIBRARY DESTINATION lib        # .so
    RUNTIME DESTINATION bin        # .dll on Windows
)

# Install headers
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# Install a single file
install(FILES README.md DESTINATION share/doc/my_project)
```

```bash
# Install to default prefix (/usr/local or C:/Program Files/...)
cmake --install build

# Install to custom prefix
cmake --install build --prefix /home/user/myapp
```

---

## Testing with CTest

```cmake
enable_testing()

add_executable(my_tests tests/test_main.cpp)
target_link_libraries(my_tests PRIVATE GTest::gtest_main mylib)

# Register individual test
add_test(NAME MyTest COMMAND my_tests)

# Register all GoogleTest cases automatically
include(GoogleTest)
gtest_discover_tests(my_tests)
```

```bash
# Run all tests
ctest --test-dir build

# Verbose output
ctest --test-dir build -V

# Run only tests matching a pattern
ctest --test-dir build -R "MyTest*"

# Run with parallel jobs
ctest --test-dir build -j 4
```

---

## Generator Expressions

Evaluated at build time (not configure time). Syntax: `$<...>`

```cmake
# Configuration-dependent flags
target_compile_options(my_app PRIVATE
    $<$<CONFIG:Debug>:   -g -O0>
    $<$<CONFIG:Release>: -O3 -DNDEBUG>
)

# Compiler-dependent flags
target_compile_options(my_app PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:    -Wall -Wextra>
    $<$<CXX_COMPILER_ID:Clang>:  -Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:   /W4>
)

# Platform-dependent definitions
target_compile_definitions(my_app PRIVATE
    $<$<PLATFORM_ID:Windows>: PLATFORM_WINDOWS>
    $<$<PLATFORM_ID:Linux>:   PLATFORM_LINUX>
)

# BUILD vs INSTALL include paths
target_include_directories(mylib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
```

---

## Toolchain & Cross Compilation

### Toolchain File

Create `toolchain-cygwin64.cmake`:

```cmake
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER   E:/Installation/CTools/cygwin64/bin/gcc.exe)
set(CMAKE_CXX_COMPILER E:/Installation/CTools/cygwin64/bin/g++.exe)

set(CMAKE_FIND_ROOT_PATH E:/Installation/CTools/cygwin64)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
```

```bash
cmake -S . -B build --toolchain toolchain-cygwin64.cmake
```

---

## Advanced Patterns

### Precompiled Headers

```cmake
target_precompile_headers(my_app PRIVATE pch.h)

# Share PCH between targets (avoids recompiling)
target_precompile_headers(my_app REUSE_FROM mylib)
```

### Unity Builds (faster compilation)

```cmake
set_target_properties(my_app PROPERTIES UNITY_BUILD ON)
```

### compile_commands.json (for clangd / IDE tooling)

```cmake
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

Or from CLI:
```bash
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

### Custom Commands & Targets

```cmake
# Run a command during build
add_custom_command(
    OUTPUT  ${CMAKE_BINARY_DIR}/generated.cpp
    COMMAND python3 generate.py -o ${CMAKE_BINARY_DIR}/generated.cpp
    DEPENDS generate.py
    COMMENT "Generating source..."
)

# Custom target (always runs, like 'make docs')
add_custom_target(docs
    COMMAND doxygen Doxyfile
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating documentation"
)
```

### Versioned Shared Libraries

```cmake
add_library(mylib SHARED mylib.cpp)

set_target_properties(mylib PROPERTIES
    VERSION   1.2.3
    SOVERSION 1       # libmylib.so.1
)
```

### CMake Presets (`CMakePresets.json`)

```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "debug",
      "binaryDir": "${sourceDir}/build/debug",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
      }
    },
    {
      "name": "release",
      "binaryDir": "${sourceDir}/build/release",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Release"
      }
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

---

## Quick Reference Cheat Sheet

| Task | Command |
|---|---|
| Configure | `cmake -S . -B build` |
| Build | `cmake --build build` |
| Build Release | `cmake --build build --config Release` |
| Clean | `cmake --build build --target clean` |
| Install | `cmake --install build` |
| Run tests | `ctest --test-dir build` |
| Delete cache | `rm build/CMakeCache.txt` |
| Use Ninja | `cmake -S . -B build -G Ninja` |
| Set compiler | `cmake -S . -B build -DCMAKE_CXX_COMPILER=g++` |
| Use preset | `cmake --preset release` |
