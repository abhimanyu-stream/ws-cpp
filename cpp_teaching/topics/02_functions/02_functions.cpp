/**
 * TOPIC 02 — Functions in C++
 * ============================
 * Covers: declaration/definition, pass by value/ref/ptr,
 *         default args, overloading, inline, recursion,
 *         lambdas, variadic, constexpr, trailing return
 *
 * Compile: g++ -std=c++17 -Wall 02_functions.cpp -o functions
 */

#include <iostream>
#include <string>
#include <vector>
#include <numeric>    // std::accumulate
#include <functional> // std::function

// ── Declaration vs Definition ────────────────────────────────────────────────
// Declaration — tells the compiler the function exists (can be in header)
int add(int a, int b);

// Definition — the actual body
int add(int a, int b) {
    return a + b;
}

// ── Pass by Value ─────────────────────────────────────────────────────────────
// A COPY of the argument is made. Changes inside don't affect the caller.
void double_val(int x) {
    x *= 2;   // modifies the copy, not the original
}

// ── Pass by Reference ────────────────────────────────────────────────────────
// No copy. The function works directly on the caller's variable.
void double_ref(int& x) {
    x *= 2;   // modifies the caller's variable
}

// ── Pass by Pointer ──────────────────────────────────────────────────────────
// Pass the address. Can be null — always check before dereferencing.
void double_ptr(int* x) {
    if (x) *x *= 2;
}

void section_pass_modes() {
    std::cout << "\n=== Pass Modes ===\n";

    int a = 10, b = 10, c = 10;
    double_val(a);  std::cout << "by value: "   << a << " (unchanged)\n"; // 10
    double_ref(b);  std::cout << "by ref:   "   << b << " (changed)\n";   // 20
    double_ptr(&c); std::cout << "by ptr:   "   << c << " (changed)\n";   // 20

    // When to use which:
    // by value  — small/trivial types (int, double), don't need to modify
    // by ref    — large objects (read or modify), mandatory out-parameters
    // by ptr    — optional parameters (can be nullptr), C interop
    // const ref — large objects, read-only
}

// ── Default Arguments ────────────────────────────────────────────────────────
// Default values must be in the declaration, trailing parameters only.
void greet(const std::string& name, const std::string& greeting = "Hello") {
    std::cout << greeting << ", " << name << "!\n";
}

// ── Function Overloading ──────────────────────────────────────────────────────
// Same name, different parameter types or count.
// Return type alone is NOT enough to overload.
int    max_val(int a, int b)       { return a > b ? a : b; }
double max_val(double a, double b) { return a > b ? a : b; }
int    max_val(int a, int b, int c){ return max_val(max_val(a, b), c); }

void section_overloading() {
    std::cout << "\n=== Overloading ===\n";
    std::cout << "max(3,7):       " << max_val(3, 7)       << "\n"; // calls int version
    std::cout << "max(3.1,2.9):   " << max_val(3.1, 2.9)   << "\n"; // calls double version
    std::cout << "max(1,5,3):     " << max_val(1, 5, 3)     << "\n"; // calls 3-param version
}

// ── Inline Functions ──────────────────────────────────────────────────────────
// Suggests the compiler replace the call with the function body.
// Good for tiny functions to avoid call overhead.
// Modern compilers often inline anyway — this is a hint, not a command.
inline int cube(int x) { return x * x * x; }

// ── Recursion ────────────────────────────────────────────────────────────────
// A function calling itself. Needs a base case to stop.
// Each call uses stack space — deep recursion can stack overflow.

long long factorial(int n) {
    // Base case — stops recursion
    if (n <= 1) return 1;
    // Recursive case
    return n * factorial(n - 1);
}

// Fibonacci — classic recursion (inefficient without memoization)
int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

void section_recursion() {
    std::cout << "\n=== Recursion ===\n";
    for (int i = 0; i <= 10; ++i)
        std::cout << i << "! = " << factorial(i) << "\n";

    std::cout << "fib(0..8): ";
    for (int i = 0; i <= 8; ++i) std::cout << fibonacci(i) << " ";
    std::cout << "\n";
}

// ── Lambda Functions ──────────────────────────────────────────────────────────
// Anonymous functions defined inline.
// Syntax: [capture](params) -> return_type { body }
void section_lambdas() {
    std::cout << "\n=== Lambdas ===\n";

    // Basic lambda
    auto add_l = [](int a, int b) { return a + b; };
    std::cout << "lambda add: " << add_l(3, 4) << "\n";

    // Capture by value [=]
    int factor = 5;
    auto multiply = [factor](int x) { return x * factor; };
    std::cout << "multiply by 5: " << multiply(6) << "\n";
    factor = 100;  // doesn't affect multiply — captured by value at definition

    // Capture by reference [&]
    int total = 0;
    auto accumulate = [&total](int x) { total += x; };
    for (int i = 1; i <= 5; ++i) accumulate(i);
    std::cout << "total 1..5: " << total << "\n";   // 15

    // Mutable lambda — can modify value-captured variable
    auto counter = [n = 0]() mutable { return ++n; };
    std::cout << counter() << " " << counter() << " " << counter() << "\n"; // 1 2 3

    // Lambda stored in std::function
    std::function<int(int)> square = [](int x) { return x * x; };
    std::cout << "square(7): " << square(7) << "\n";

    // Generic lambda (C++14) — auto parameters
    auto print_twice = [](auto val) {
        std::cout << val << " " << val << "\n";
    };
    print_twice(42);
    print_twice("hello");
    print_twice(3.14);
}

// ── Variadic Functions ────────────────────────────────────────────────────────
// Accept any number of arguments.
// C-style (va_list) — old way, type-unsafe.
// Template variadic — modern, type-safe (see templates topic).

#include <cstdarg>
double c_sum(int count, ...) {
    va_list args;
    va_start(args, count);
    double total = 0;
    for (int i = 0; i < count; ++i)
        total += va_arg(args, double);
    va_end(args);
    return total;
}

// Modern variadic using fold expressions (C++17)
template<typename... Args>
auto modern_sum(Args... args) {
    return (args + ...);   // fold expression
}

void section_variadic() {
    std::cout << "\n=== Variadic ===\n";
    std::cout << "c_sum(3, 1.0,2.0,3.0): " << c_sum(3, 1.0, 2.0, 3.0) << "\n";
    std::cout << "modern_sum(1,2,3,4,5):  " << modern_sum(1, 2, 3, 4, 5) << "\n";
    std::cout << "modern_sum(1.5,2.5):    " << modern_sum(1.5, 2.5)      << "\n";
}

// ── constexpr Functions ───────────────────────────────────────────────────────
// Can be evaluated at compile time when given constexpr arguments.
constexpr long long power(long long base, int exp) {
    if (exp == 0) return 1;
    return base * power(base, exp - 1);
}

// ── consteval Functions (C++20) ───────────────────────────────────────────────
// MUST be evaluated at compile time — runtime call is an error.
// consteval int compile_only(int x) { return x * x; }

// ── Trailing Return Type ──────────────────────────────────────────────────────
// Return type written after -> instead of before the name.
// Needed when return type depends on parameters.
auto divide(double a, double b) -> double {
    return a / b;
}

template<typename T, typename U>
auto multiply_t(T a, U b) -> decltype(a * b) {
    return a * b;   // return type deduced: int*double = double
}

// ── Function Templates ────────────────────────────────────────────────────────
// (Full coverage in Topic 08. Brief intro here.)
template<typename T>
T clamp(T val, T lo, T hi) {
    if (val < lo) return lo;
    if (val > hi) return hi;
    return val;
}

void section_templates_intro() {
    std::cout << "\n=== Function Template Intro ===\n";
    std::cout << "clamp(15, 0, 10): " << clamp(15, 0, 10)       << "\n"; // 10
    std::cout << "clamp(5,  0, 10): " << clamp(5, 0, 10)        << "\n"; // 5
    std::cout << "clamp(3.14,0.0,3.0): " << clamp(3.14,0.0,3.0) << "\n"; // 3.0
}

int main() {
    std::cout << "╔══ Topic 02: Functions ══╗\n";

    section_pass_modes();

    std::cout << "\n=== Default Arguments ===\n";
    greet("Alice");                  // Hello, Alice!
    greet("Bob", "Good morning");    // Good morning, Bob!

    section_overloading();

    std::cout << "\n=== Inline & constexpr ===\n";
    std::cout << "cube(4):         " << cube(4)             << "\n";  // 64
    std::cout << "2^10 constexpr:  " << power(2, 10)        << "\n";  // 1024
    std::cout << "divide(10, 3):   " << divide(10, 3)        << "\n";
    std::cout << "multiply(3,4.5): " << multiply_t(3, 4.5)   << "\n"; // double

    section_recursion();
    section_lambdas();
    section_variadic();
    section_templates_intro();

    std::cout << "\n✓ Topic 02 complete.\n";
}
