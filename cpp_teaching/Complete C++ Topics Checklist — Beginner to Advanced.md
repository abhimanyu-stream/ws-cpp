Complete C++ Topics Checklist — Beginner to Advanced
Below is a comprehensive C++ learning roadmap covering core programming, pointers, memory management, function pointers, templates (all major types), OOP, STL, modern C++ (C++11–C++23), multithreading, and advanced system programming.

The roadmap is suitable for software developers preparing for C++ developer, code reviewer, systems programming, and technical interview roles.

---

## ✅ Implementation Status

All 18 topics have been implemented as self-contained `.cpp` programs with inline teaching notes.

| Topic | File | Status |
|-------|------|--------|
| 01 C++ Fundamentals | `topics/01_fundamentals/01_fundamentals.cpp` | ✅ Done |
| 02 Functions | `topics/02_functions/02_functions.cpp` | ✅ Done |
| 03 Pointers | `topics/03_pointers/03_pointers.cpp` | ✅ Done |
| 04 Function Pointers | `topics/04_function_pointers/04_function_pointers.cpp` | ✅ Done |
| 05 References | `topics/05_references/05_references.cpp` | ✅ Done |
| 06 Arrays & Strings | `topics/06_arrays_and_strings/06_arrays_strings.cpp` | ✅ Done |
| 07 OOP | `topics/07_oop/07_oop.cpp` | ✅ Done |
| 08 Templates | `topics/08_templates/08_templates.cpp` | ✅ Done |
| 09 STL | `topics/09_stl/09_stl.cpp` | ✅ Done |
| 10 Memory & Smart Pointers | `topics/10_memory_smart_pointers/10_memory_smart_pointers.cpp` | ✅ Done |
| 11 Exception Handling | `topics/11_exception_handling/11_exception_handling.cpp` | ✅ Done |
| 12 Modern C++ (C++11–C++23) | `topics/12_modern_cpp/12_modern_cpp.cpp` | ✅ Done |
| 13 Multithreading | `topics/13_multithreading/13_multithreading.cpp` | ✅ Done |
| 14 File Handling | `topics/14_file_handling/14_file_handling.cpp` | ✅ Done |
| 15 Advanced C++ | `topics/15_advanced_cpp/15_advanced_cpp.cpp` | ✅ Done |
| 16 Design Patterns | `topics/16_design_patterns/16_design_patterns.cpp` | ✅ Done |
| 17 Data Structures & Algorithms | `topics/17_data_structures_algorithms/17_dsa.cpp` | ✅ Done |
| 18 Tools & Build Systems | `topics/18_tools_build_systems/18_tools_build_systems.md` | ✅ Done |

### Companion Reference Docs
| File | Topic |
|------|-------|
| `LVALUE_RVALUE.md` | lvalue, rvalue, move semantics, RVO |
| `CONST_GUIDE.md` | All const variants, how to read declarations |
| `REFERENCES_T_REF_VS_TREF_REF.md` | T& vs T&&, universal refs, std::forward |
| `CMAKE_GUIDE.md` | CMake basic to advanced |
| `src/00_cpp17_syntax_guide.cpp` | All C++17 syntax in one file |
| `topics/README.md` | Index of all topics with compile instructions |

---

1. C++ Fundamentals
- Introduction to C++
- History and evolution of C++
- C vs C++
- C++ compilation process
- Preprocessor directives
- Header files and source files
- Namespaces
- Variables and constants
- Data types
- Type modifiers (signed, unsigned, short, long)
- Type conversion and type casting
- Operators and expressions
- Operator precedence and associativity
- Conditional statements
- Loops (for, while, do-while)
- break, continue, goto
- Input/output (cin, cout, cerr)
- const and constexpr
- auto and decltype
- Scope and lifetime of variables
2. Functions in C++
- Function declaration and definition
- Function parameters and arguments
- Pass by value
- Pass by reference
- Pass by pointer
- Default arguments
- Function overloading
- Inline functions
- Recursive functions
- Lambda functions
- Variadic functions
- constexpr functions
- consteval functions
- Function return types
- Trailing return types
- Function templates
3. Pointers — Complete Coverage
Important for C++ interviews
- Introduction to pointers
- Pointer declaration and initialization
- Address-of operator (&)
- Dereference operator (*)
- Null pointers
- nullptr
- Void pointers
- Wild pointers
- Dangling pointers
- Pointer arithmetic
- Pointer comparison
- Pointer to pointer (int **ptr)
- Triple pointers (int ***ptr)
- Constant pointers
- Pointer to constant
- Constant pointer to constant
- Pointers and arrays
- Pointers and strings
- Pointers and structures
- Pointers and classes
- Dynamic memory allocation (new, delete)
- Dynamic arrays (new[], delete[])
- Memory leaks
- Shallow copy vs deep copy
4. Function Pointers — Advanced
Important
- Introduction to function pointers
- Declaration of function pointers
- Initialization of function pointers
- Calling functions using pointers
- Function pointer as function argument
- Function pointer as return type
- Array of function pointers
- Pointer to overloaded functions
- Function pointers with classes
- Function pointers with structures
- Function pointers and callbacks
- Function pointers and event handling
- Function pointers and dynamic dispatch
- Function pointers vs virtual functions
- Function pointers vs lambda expressions
- Function pointers vs std::function
- Function pointers vs std::bind
- Member function pointers
- Pointer to static member functions
- Pointer to non-static member functions
Example:
#include <iostream>
using namespace std;

int add(int a, int b) {
    return a + b;
}

int main() {
    int (*operation)(int, int) = add;

    cout << operation(10, 20);
}


5. References in C++
- Lvalue references
- Rvalue references
- Reference initialization
- Reference vs pointer
- Reference to constant
- Reference to pointer
- Pointer to reference (why it is invalid)
- Reference to array
- Reference to function
- Returning references
- Reference collapsing
- Universal references / forwarding references
- Perfect forwarding
- std::move
- std::forward
6. Arrays and Strings
- One-dimensional arrays
- Two-dimensional arrays
- Multidimensional arrays
- Dynamic arrays
- Array of pointers
- Pointer to array
- Array of function pointers
- Character arrays
- C-style strings
- String manipulation
- std::string
- std::string_view
- String literals
- Raw string literals
- Array decay
- std::array
- std::span
7. Object-Oriented Programming (OOP)
- Classes and objects
- Access specifiers
- Encapsulation
- Abstraction
- Inheritance
- Polymorphism
- Compile-time polymorphism
- Runtime polymorphism
- Constructors
- Default constructor
- Parameterized constructor
- Copy constructor
- Move constructor
- Destructor
- Constructor overloading
- Constructor delegation
- Static data members
- Static member functions
- this pointer
- Friend functions
- Friend classes
- Nested classes
- Abstract classes
- Pure virtual functions
- Virtual functions
- Virtual destructors
- Function overriding
- Function hiding
- Operator overloading
- Multiple inheritance
- Multilevel inheritance
- Hierarchical inheritance
- Hybrid inheritance
- Diamond problem
- Virtual inheritance
- Object slicing
- Rule of Three
- Rule of Five
- Rule of Zero
8. C++ Templates — Complete Coverage
Very important
This is one of the most important areas for advanced C++ development.
8.1 Function Templates
- Basic function templates
- Function template parameters
- Multiple template parameters
- Template argument deduction
- Explicit template arguments
- Function template specialization
- Function template overloading
- Variadic function templates
- Recursive variadic templates
- Fold expressions
- Perfect forwarding with templates
Example:
template <typename T>
T add(T a, T b) {
    return a + b;
}


8.2 Class Templates
- Basic class templates
- Multiple template parameters
- Default template parameters
- Template classes with constructors
- Template classes with inheritance
- Template classes with member functions
- Template class specialization
- Partial specialization
- Full specialization
- Nested class templates
- Static members in class templates
- Friend functions in templates
Example:
template <typename T>
class Container {
private:
    T value;

public:
    Container(T v) : value(v) {}

    T getValue() {
        return value;
    }
};


8.3 Types of Template Parameters
Template parameter	Description
Type parameters	typename T, class T
Non-type parameters	int N, bool B
Template template parameters	Template passed to another template
Variadic parameters	typename... Args
Constrained parameters	C++20 concepts
Auto template parameters	auto N

8.4 Advanced Template Concepts
- Template specialization
- Partial template specialization
- Explicit specialization
- Template instantiation
- Implicit instantiation
- Explicit instantiation
- Template argument deduction
- SFINAE
- std::enable_if
- Type traits
- Compile-time computation
- Template metaprogramming
- Recursive template instantiation
- Variadic templates
- Fold expressions
- CRTP (Curiously Recurring Template Pattern)
- Concepts and constraints
- requires expressions
- if constexpr
- Type deduction with decltype
- std::conditional
- std::is_same
- std::remove_reference
- std::decay
9. STL — Standard Template Library
9.1 Containers
Container category	Topics
Sequence	vector, list, deque, array, forward_list
Associative	set, multiset, map, multimap
Unordered	unordered_set, unordered_map, unordered_multiset, unordered_multimap
Container adapters	stack, queue, priority_queue

9.2 Iterators
- Input iterators
- Output iterators
- Forward iterators
- Bidirectional iterators
- Random access iterators
- Contiguous iterators
- Reverse iterators
- Constant iterators
- Iterator invalidation
9.3 STL Algorithms
- sort()
- stable_sort()
- binary_search()
- lower_bound()
- upper_bound()
- find()
- find_if()
- count()
- count_if()
- reverse()
- rotate()
- unique()
- remove()
- transform()
- accumulate()
- for_each()
- min_element()
- max_element()
- partition()
- merge()
9.4 STL Utilities
- pair
- tuple
- optional
- variant
- any
- function
- bind
- reference_wrapper
- utility
- numeric
- type_traits
10. Memory Management and Smart Pointers
- Stack memory
- Heap memory
- Static memory
- Dynamic memory allocation
- Memory layout
- Memory alignment
- Padding
- Memory fragmentation
- Memory leaks
- Dangling references
- RAII
- Ownership semantics
Smart pointers:
- unique_ptr
- shared_ptr
- weak_ptr
- make_unique
- make_shared
- Custom deleters
- Circular references
- Reference counting
- Move semantics
- Ownership transfer
11. Exception Handling
- Exceptions
- try
- catch
- throw
- Multiple catch blocks
- Catch-all handler
- Custom exception classes
- Exception propagation
- Stack unwinding
- Exception safety guarantees
- noexcept
- RAII and exceptions
- Standard exception hierarchy
12. Modern C++ (C++11 to C++23)
Version	Important features
C++11	Lambda, auto, nullptr, move semantics, smart pointers, variadic templates
C++14	Generic lambdas, variable templates, relaxed constexpr
C++17	Structured bindings, if constexpr, optional, variant, any, filesystem
C++20	Concepts, ranges, coroutines, modules, three-way comparison, concepts
C++23	std::expected, std::print, ranges improvements, deducing this

13. Multithreading and Concurrency
- Processes vs threads
- Thread creation
- std::thread
- Thread joining and detaching
- Mutex
- Recursive mutex
- Timed mutex
- Shared mutex
- Lock guards
- Unique locks
- Condition variables
- Atomic variables
- Memory ordering
- Thread safety
- Race conditions
- Deadlocks
- Starvation
- Futures and promises
- std::async
- Thread pools
- Producer-consumer pattern
- Parallel algorithms
- Coroutines
14. File Handling and Serialization
- File streams
- ifstream
- ofstream
- fstream
- Binary file operations
- Text file operations
- File pointers
- File positioning
- Reading and writing structures
- Serialization
- Deserialization
- JSON integration
- XML integration
- Binary serialization
15. Advanced C++ Programming
- C++ memory model
- Undefined behavior
- Implementation-defined behavior
- Unspecified behavior
- Object lifetime
- Storage duration
- Linkage
- Name mangling
- One Definition Rule (ODR)
- Templates and compilation
- ABI compatibility
- Dynamic linking
- Static linking
- Shared libraries
- Virtual tables (vtable)
- Virtual pointer (vptr)
- RTTI
- dynamic_cast
- static_cast
- const_cast
- reinterpret_cast
- Placement new
- Custom allocators
- Memory pools
- Lock-free programming
- Atomic operations
- Cache locality
- False sharing
- SIMD programming
16. Design Patterns in C++
- Singleton
- Factory
- Abstract Factory
- Builder
- Prototype
- Adapter
- Decorator
- Facade
- Proxy
- Observer
- Strategy
- Command
- State
- Iterator
- Visitor
- Dependency Injection
- RAII design pattern
17. Data Structures and Algorithms using C++
- Arrays
- Linked lists
- Stacks
- Queues
- Hash tables
- Trees
- Binary search trees
- AVL trees
- Red-black trees
- Heaps
- Tries
- Graphs
- BFS
- DFS
- Sorting algorithms
- Searching algorithms
- Recursion
- Backtracking
- Dynamic programming
- Greedy algorithms
- Sliding window
- Two pointers
- Bit manipulation
- Time complexity
- Space complexity
18. C++ Tools and Build Systems
- GCC
- G++
- Clang
- MSVC
- GDB
- LLDB
- CMake
- Make
- Ninja
- vcpkg
- Conan
- Google Test
- Catch2
- AddressSanitizer
- ThreadSanitizer
- Valgrind
- Static analysis
- Clang-Tidy
- Clang-Format
- C++ unit testing
- Profiling and benchmarking

Recommended learning priority
For a developer targeting C++ code-review and advanced programming roles, I recommend this order:
Priority	Topics
1	Fundamentals, functions, arrays
2	Pointers, references, memory management
3	Function pointers and callbacks
4	OOP and polymorphism
5	Templates and template metaprogramming
6	STL containers, iterators and algorithms
7	Smart pointers, RAII and move semantics
8	Modern C++11–C++23
9	Multithreading and concurrency
10	Advanced C++, debugging and code review
