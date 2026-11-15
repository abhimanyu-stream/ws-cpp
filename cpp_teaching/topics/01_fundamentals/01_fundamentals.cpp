/**
 * TOPIC 01 — C++ Fundamentals
 * ============================
 * Covers: variables, types, operators, control flow,
 *         type conversion, namespaces, const, auto, scope
 *
 * Compile: g++ -std=c++17 -Wall 01_fundamentals.cpp -o fundamentals
 */

#include <iostream>
#include <string>
#include <typeinfo>   // typeid
#include <climits>    // INT_MAX, INT_MIN
#include <cfloat>     // DBL_MAX

// ── Namespaces ───────────────────────────────────────────────────────────────
// Namespaces prevent name collisions between libraries and your code.
namespace math {
    const double PI = 3.14159265358979;
    double circle_area(double r) { return PI * r * r; }
}

namespace io {
    void greet(const std::string& name) {
        std::cout << "Hello, " << name << "!\n";
    }
}

// ── Variables and Data Types ─────────────────────────────────────────────────
void section_types() {
    std::cout << "\n=== Variables & Data Types ===\n";

    // Integer types — size varies by platform
    int           i  =  42;        // typically 4 bytes
    short         s  =  100;       // at least 2 bytes
    long          l  =  1000000L;  // at least 4 bytes
    long long     ll = 9876543210LL; // at least 8 bytes
    unsigned int  u  =  42U;       // no negative values

    // Floating point
    float         f  = 3.14f;      // 4 bytes, ~7 decimal digits
    double        d  = 3.14159;    // 8 bytes, ~15 decimal digits
    long double   ld = 3.14159L;   // 16 bytes on most platforms

    // Characters
    char          c  = 'A';        // 1 byte
    wchar_t       wc = L'Ω';       // wide character
    char16_t      c16= u'α';       // UTF-16
    char32_t      c32= U'😀';      // UTF-32

    // Boolean
    bool          b  = true;       // true (1) or false (0)

    std::cout << "int max: "       << INT_MAX   << "\n";
    std::cout << "int min: "       << INT_MIN   << "\n";
    std::cout << "sizeof int: "    << sizeof(i) << " bytes\n";
    std::cout << "sizeof double: " << sizeof(d) << " bytes\n";
    std::cout << "char 'A' = "     << c         << " (ascii " << (int)c << ")\n";
    std::cout << "bool true  = "   << b         << "\n";

    (void)s; (void)l; (void)ll; (void)u; (void)f; (void)ld;
    (void)wc; (void)c16; (void)c32;
}

// ── Type Modifiers ───────────────────────────────────────────────────────────
void section_type_modifiers() {
    std::cout << "\n=== Type Modifiers ===\n";

    // signed — can hold negative (default for int, char)
    signed int   si = -100;

    // unsigned — only positive, doubles the positive range
    unsigned int ui = 4294967295U;   // 2^32 - 1

    // short — at least 2 bytes
    short int    sh = 32767;

    // long — at least 4 bytes (8 on 64-bit Linux)
    long int     lo = 2147483647L;

    std::cout << "signed int:   " << si << "\n";
    std::cout << "unsigned int: " << ui << "\n";
    std::cout << "short:        " << sh << "\n";
    std::cout << "long:         " << lo << "\n";

    // Overflow demonstration
    unsigned char uc = 255;
    ++uc;  // wraps around to 0 (defined for unsigned)
    std::cout << "uchar 255 + 1 = " << (int)uc << " (wraps)\n";
}

// ── Type Conversion & Casting ────────────────────────────────────────────────
void section_casting() {
    std::cout << "\n=== Type Conversion & Casting ===\n";

    // Implicit conversion (widening — safe)
    int    i = 42;
    double d = i;      // int → double, no data loss
    std::cout << "int " << i << " → double " << d << "\n";

    // Implicit narrowing (may lose data — compiler may warn)
    double pi  = 3.14159;
    int    pi_i = (int)pi;   // C-style cast — truncates to 3
    std::cout << "double " << pi << " → int " << pi_i << " (truncated)\n";

    // C++ named casts — prefer these over C-style casts

    // static_cast — compile-time checked conversion
    double  x     = 9.99;
    int     xi    = static_cast<int>(x);        // explicit truncation
    float   xf    = static_cast<float>(x);      // narrowing, explicit
    std::cout << "static_cast<int>(9.99) = " << xi << "\n";

    // reinterpret_cast — raw bitwise reinterpretation (dangerous)
    int    n   = 65;
    char*  cp  = reinterpret_cast<char*>(&n);
    std::cout << "reinterpret_cast int 65 as char: " << *cp << "\n"; // 'A' on little-endian

    // const_cast — add/remove const (use rarely)
    const int  ci  = 100;
    const int* cip = &ci;
    // int* ip = const_cast<int*>(cip);  // removes const — UB if original is const!
    // *ip = 200;                         // UNDEFINED BEHAVIOR

    (void)xf;
}

// ── auto and decltype ────────────────────────────────────────────────────────
void section_auto_decltype() {
    std::cout << "\n=== auto & decltype ===\n";

    // auto — compiler deduces type from initializer
    auto a  = 42;           // int
    auto b  = 3.14;         // double
    auto c  = true;         // bool
    auto s  = std::string("hello");  // std::string
    auto&  r  = a;          // int& — reference, deduced with &

    std::cout << "auto int: "    << a << "\n";
    std::cout << "auto double: " << b << "\n";

    // decltype — query the type of an expression without evaluating it
    decltype(a)      x = 10;     // x is int (same type as a)
    decltype(a + b)  y = 1.5;    // y is double (int + double → double)
    decltype(r)      z = a;      // z is int& (same type as r — reference!)

    std::cout << "decltype(a+b): " << y << "\n";

    z = 99;   // modifies 'a' because z is int&
    std::cout << "a after z=99: " << a << "\n";

    (void)c; (void)s; (void)x;
}

// ── const and constexpr ──────────────────────────────────────────────────────
void section_const_constexpr() {
    std::cout << "\n=== const & constexpr ===\n";

    // const — runtime constant, cannot be changed after initialization
    const int max_size = 100;
    // max_size = 200;  // ERROR

    // constexpr — compile-time constant, evaluated by the compiler
    constexpr double PI = 3.14159265358979;
    constexpr int    ARR_SIZE = 10;
    int              arr[ARR_SIZE];     // OK — size known at compile time

    // constexpr function — computed at compile time when inputs are constexpr
    constexpr auto square = [](int n) constexpr { return n * n; };
    constexpr int s = square(5);   // 25 baked into binary
    int           t = square(5);   // also OK at runtime

    std::cout << "max_size: "  << max_size << "\n";
    std::cout << "PI: "        << PI       << "\n";
    std::cout << "square(5): " << s        << "\n";

    (void)arr; (void)t;
}

// ── Operators ────────────────────────────────────────────────────────────────
void section_operators() {
    std::cout << "\n=== Operators ===\n";

    int a = 10, b = 3;

    // Arithmetic
    std::cout << a << " + " << b << " = " << (a + b) << "\n";
    std::cout << a << " - " << b << " = " << (a - b) << "\n";
    std::cout << a << " * " << b << " = " << (a * b) << "\n";
    std::cout << a << " / " << b << " = " << (a / b) << " (integer division)\n";
    std::cout << a << " % " << b << " = " << (a % b) << " (modulo/remainder)\n";

    // Increment / decrement
    int x = 5;
    std::cout << "x++: " << x++ << " (returns 5, then x=6)\n";  // post: return then increment
    std::cout << "++x: " << ++x << " (increments then returns 7)\n";  // pre: increment then return

    // Comparison — return bool
    std::cout << (a == b) << " " << (a != b) << " " << (a > b) << " " << (a < b) << "\n";

    // Logical
    bool p = true, q = false;
    std::cout << "AND: " << (p && q) << "  OR: " << (p || q) << "  NOT: " << (!p) << "\n";

    // Bitwise
    std::cout << "5 & 3 = "  << (5 & 3)  << "  (AND)\n";     // 101 & 011 = 001 = 1
    std::cout << "5 | 3 = "  << (5 | 3)  << "  (OR)\n";      // 101 | 011 = 111 = 7
    std::cout << "5 ^ 3 = "  << (5 ^ 3)  << "  (XOR)\n";     // 101 ^ 011 = 110 = 6
    std::cout << "~5 = "     << (~5)      << "  (NOT)\n";     // flip all bits
    std::cout << "1 << 3 = " << (1 << 3) << "  (left shift = 1*2^3)\n";
    std::cout << "8 >> 2 = " << (8 >> 2) << "  (right shift = 8/2^2)\n";

    // Ternary
    int max = (a > b) ? a : b;
    std::cout << "max(" << a << "," << b << ") = " << max << "\n";
}

// ── Control Flow ─────────────────────────────────────────────────────────────
void section_control_flow() {
    std::cout << "\n=== Control Flow ===\n";

    // if / else if / else
    int score = 75;
    if      (score >= 90) std::cout << "Grade: A\n";
    else if (score >= 75) std::cout << "Grade: B\n";
    else if (score >= 60) std::cout << "Grade: C\n";
    else                  std::cout << "Grade: F\n";

    // switch — works with int, char, enum
    char grade = 'B';
    switch (grade) {
        case 'A': std::cout << "Excellent\n"; break;
        case 'B': std::cout << "Good\n";      break;
        case 'C': std::cout << "Average\n";   break;
        default:  std::cout << "Unknown\n";   break;
    }

    // for loop
    std::cout << "for: ";
    for (int i = 0; i < 5; ++i) std::cout << i << " ";
    std::cout << "\n";

    // while loop
    std::cout << "while: ";
    int n = 0;
    while (n < 5) { std::cout << n++ << " "; }
    std::cout << "\n";

    // do-while — always executes at least once
    std::cout << "do-while: ";
    int m = 0;
    do { std::cout << m++ << " "; } while (m < 5);
    std::cout << "\n";

    // break — exits loop
    std::cout << "break at 3: ";
    for (int i = 0; i < 10; ++i) {
        if (i == 3) break;
        std::cout << i << " ";
    }
    std::cout << "\n";

    // continue — skips rest of loop body
    std::cout << "skip even: ";
    for (int i = 0; i < 8; ++i) {
        if (i % 2 == 0) continue;
        std::cout << i << " ";
    }
    std::cout << "\n";

    // Range-based for (C++11)
    int arr[] = {10, 20, 30, 40};
    std::cout << "range-for: ";
    for (const auto& v : arr) std::cout << v << " ";
    std::cout << "\n";
}

// ── Scope and Lifetime ───────────────────────────────────────────────────────
void section_scope() {
    std::cout << "\n=== Scope & Lifetime ===\n";

    int x = 1;   // outer x
    {
        int x = 2;   // inner x — shadows outer
        std::cout << "inner x = " << x << "\n";   // 2
    }
    std::cout << "outer x = " << x << "\n";   // 1 — inner x is gone

    // Static local variable — created once, persists across calls
    auto counter = []() {
        static int count = 0;   // initialized once
        return ++count;
    };
    std::cout << counter() << " " << counter() << " " << counter() << "\n"; // 1 2 3
}

// ── Input / Output ───────────────────────────────────────────────────────────
void section_io() {
    std::cout << "\n=== Input / Output ===\n";

    // cout — standard output
    std::cout << "Value: " << 42 << "\n";
    std::cout << std::boolalpha << true << "\n";   // prints "true" not "1"

    // cerr — standard error (unbuffered, for errors)
    std::cerr << "This is an error message\n";

    // clog — standard log (buffered)
    std::clog << "This is a log message\n";

    // cin — standard input (skipped in this demo to avoid blocking)
    // int n;
    // std::cin >> n;

    // Formatted output
    std::cout << std::fixed;
    std::cout.precision(4);
    std::cout << "Pi = " << math::PI << "\n";
}

int main() {
    std::cout << "╔══ Topic 01: C++ Fundamentals ══╗\n";

    // Namespaces
    io::greet("C++ Learner");
    std::cout << "Circle area r=5: " << math::circle_area(5.0) << "\n";

    section_types();
    section_type_modifiers();
    section_casting();
    section_auto_decltype();
    section_const_constexpr();
    section_operators();
    section_control_flow();
    section_scope();
    section_io();

    std::cout << "\n✓ Topic 01 complete.\n";
}
