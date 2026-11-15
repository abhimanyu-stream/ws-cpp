/**
 * TOPIC 05 — References in C++
 * ==============================
 * Covers: lvalue ref, rvalue ref, const ref, ref vs ptr,
 *         ref to array/function, returning refs,
 *         reference collapsing, universal refs, perfect forwarding,
 *         std::move, std::forward
 *
 * Compile: g++ -std=c++17 -Wall 05_references.cpp -o references
 */

#include <iostream>
#include <string>
#include <vector>
#include <utility>   // std::move, std::forward

// ── Lvalue References ─────────────────────────────────────────────────────────
void section_lvalue_ref() {
    std::cout << "\n=== Lvalue References ===\n";

    int x = 10;

    // A reference is an ALIAS — it IS x, not a copy
    int& ref = x;
    ref = 99;
    std::cout << "x after ref=99: " << x << "\n";   // 99 — same object

    // Must be initialized at declaration
    // int& bad;   // ERROR — reference must be initialized

    // Cannot be reseated (made to refer to something else)
    int y = 50;
    ref = y;    // this ASSIGNS y's value to x, NOT reseating ref
    std::cout << "x after ref=y: " << x << "\n";    // 50 — x was assigned
    std::cout << "&ref == &x: " << (&ref == &x)     // 1 (true) — still same object
              << " (&ref == &y: " << (&ref == &y) << ")\n";  // 0

    // References vs pointers
    // References:  no null, no arithmetic, cleaner syntax, can't reseat
    // Pointers:    can be null, arithmetic, can reseat, can be in arrays
}

// ── Rvalue References ─────────────────────────────────────────────────────────
void section_rvalue_ref() {
    std::cout << "\n=== Rvalue References ===\n";

    // rvalue ref binds to temporaries and std::move'd objects
    int&& rr1 = 42;         // binds to literal
    int&& rr2 = 3 + 4;      // binds to expression result
    std::cout << "rr1=" << rr1 << " rr2=" << rr2 << "\n";

    int n = 10;
    // int&& bad = n;       // ERROR — n is lvalue
    int&& rr3 = std::move(n);   // OK — std::move casts to rvalue

    // KEY INSIGHT: once named, an rvalue reference IS an lvalue inside the function
    // rr1 has a name → rr1 is an lvalue here
    // To move from it, you need std::move(rr1) explicitly
    std::string s1 = "hello";
    std::string&& rs = std::move(s1);
    std::string s2 = rs;             // COPY — rs has a name, so it's lvalue here!
    std::string s3 = std::move(rs);  // MOVE — explicit cast back to rvalue

    std::cout << "s1 after move: '" << s1 << "'\n"; // empty (s3 stole it)
    std::cout << "s2 (copy): '"     << s2 << "'\n"; // "hello"
    std::cout << "s3 (move): '"     << s3 << "'\n"; // "hello"
}

// ── const T& — binds to everything ───────────────────────────────────────────
void section_const_ref() {
    std::cout << "\n=== const T& ===\n";

    // const lvalue ref can bind to lvalues AND rvalues (extends temp lifetime)
    int        a   = 5;
    const int& r1  = a;         // lvalue — OK
    const int& r2  = 42;        // rvalue literal — OK (lifetime extended)
    const int& r3  = a + 1;     // rvalue expression — OK

    std::cout << "r1=" << r1 << " r2=" << r2 << " r3=" << r3 << "\n";

    // This is why functions taking const T& accept everything:
    auto print = [](const std::string& s) { std::cout << s << "\n"; };
    std::string str = "named";
    print(str);               // lvalue
    print("temporary");       // rvalue
    print(str + " extended"); // temporary
}

// ── Reference to Pointer and Pointer to Reference ─────────────────────────────
void section_ref_to_ptr() {
    std::cout << "\n=== Reference to Pointer ===\n";

    int   x = 10;
    int*  p = &x;
    int*& rp = p;    // reference to pointer — allows modifying p itself

    // Use case: function that changes what a pointer points to
    int y = 99;
    rp = &y;    // changes p (not *p) — now p points to y
    std::cout << "*p = " << *p << "\n";   // 99 — p now points to y

    // Pointer to reference — INVALID in C++
    // int& *pr = &ref;  // ERROR — pointers to references are not allowed
    std::cout << "Pointer to reference: not valid in C++\n";
}

// ── Reference to Array ───────────────────────────────────────────────────────
void section_ref_to_array() {
    std::cout << "\n=== Reference to Array ===\n";

    int arr[5] = {1, 2, 3, 4, 5};

    // Reference to array — preserves the array type (no decay to pointer)
    int (&rArr)[5] = arr;
    //   ─────────────── reference to array of 5 ints

    rArr[0] = 99;
    std::cout << "arr[0] after rArr[0]=99: " << arr[0] << "\n";   // 99
    std::cout << "sizeof rArr: " << sizeof(rArr) << "\n";          // 20 (5 ints)

    // Function taking array by reference — knows the size
    auto sum_array = [](int (&a)[5]) {
        int total = 0;
        for (auto x : a) total += x;
        return total;
    };
    std::cout << "sum: " << sum_array(arr) << "\n";
}

// ── Reference to Function ────────────────────────────────────────────────────
int add(int a, int b) { return a + b; }

void section_ref_to_func() {
    std::cout << "\n=== Reference to Function ===\n";

    // Function reference — alias for a function (rarely used vs function pointer)
    int (&fn_ref)(int, int) = add;
    std::cout << "fn_ref(3, 4): " << fn_ref(3, 4) << "\n";   // 7

    // More commonly used as parameter type
    auto apply = [](int a, int b, int (&op)(int, int)) {
        return op(a, b);
    };
    std::cout << "apply(5,6,add): " << apply(5, 6, add) << "\n";   // 11
}

// ── Returning References ──────────────────────────────────────────────────────
int global_val = 100;

// Safe: returning reference to something that outlives the function
int& get_global() { return global_val; }

// UNSAFE: returning reference to a local variable — dangling!
// int& bad_ref() {
//     int local = 42;
//     return local;   // UNDEFINED BEHAVIOR — local is destroyed
// }

// Returning by reference enables assignment:
class IntArray {
    int data[10]{};
public:
    int& operator[](int i) { return data[i]; }  // non-const: assignable
    const int& operator[](int i) const { return data[i]; }  // const: read-only
};

void section_returning_ref() {
    std::cout << "\n=== Returning References ===\n";

    get_global() = 999;    // assigns through the reference
    std::cout << "global_val: " << global_val << "\n";   // 999

    IntArray arr;
    arr[3] = 42;   // calls non-const operator[] which returns int&
    std::cout << "arr[3]: " << arr[3] << "\n";   // 42
}

// ── std::move ────────────────────────────────────────────────────────────────
void section_move() {
    std::cout << "\n=== std::move ===\n";

    // std::move is just a cast — it doesn't move anything
    // It converts an lvalue to an rvalue reference, enabling move semantics

    std::string s1 = "expensive string with lots of data";

    std::string s2 = s1;             // COPY — s1 unchanged
    std::cout << "s1 after copy: '" << s1 << "'\n";

    std::string s3 = std::move(s1);  // MOVE — s1 is now empty
    std::cout << "s1 after move: '" << s1 << "'\n";   // ""
    std::cout << "s3: '"            << s3 << "'\n";

    // Vector move
    std::vector<int> v1 = {1, 2, 3, 4, 5};
    std::vector<int> v2 = std::move(v1);  // O(1) — just pointer swap
    std::cout << "v1 size after move: " << v1.size() << "\n";  // 0
    std::cout << "v2 size: "            << v2.size() << "\n";  // 5

    // Don't use an object after moving from it
    // v1 is in valid-but-unspecified state — reassign before use
    v1 = {10, 20};  // safe to reuse after reassignment
}

// ── Perfect Forwarding & std::forward ────────────────────────────────────────
// When you receive T&& in a template and want to pass it on,
// you must use std::forward to preserve the value category.

void target(int& x)       { std::cout << "  → lvalue ref\n"; }
void target(const int& x) { std::cout << "  → const lvalue ref\n"; }
void target(int&& x)      { std::cout << "  → rvalue ref\n"; }

template<typename T>
void broken_relay(T&& arg) {
    // 'arg' has a name → always lvalue here, even if T&& was deduced as rvalue ref
    target(arg);    // ALWAYS calls lvalue version!
}

template<typename T>
void correct_relay(T&& arg) {
    // std::forward<T> restores the original value category
    target(std::forward<T>(arg));
}

void section_forward() {
    std::cout << "\n=== std::forward & Perfect Forwarding ===\n";

    int n = 5;

    std::cout << "broken_relay(n):            "; broken_relay(n);           // lvalue
    std::cout << "broken_relay(42):           "; broken_relay(42);          // lvalue! (wrong)
    std::cout << "broken_relay(std::move(n)): "; broken_relay(std::move(n)); // lvalue! (wrong)

    std::cout << "correct_relay(n):            "; correct_relay(n);           // lvalue
    std::cout << "correct_relay(42):           "; correct_relay(42);          // rvalue ✓
    std::cout << "correct_relay(std::move(n)): "; correct_relay(std::move(n)); // rvalue ✓
}

// ── Reference Collapsing ──────────────────────────────────────────────────────
// When template deduction creates a reference to a reference, they collapse:
//   T& &   → T&
//   T& &&  → T&
//   T&& &  → T&
//   T&& && → T&&   (only & && stays &&)
// This is what makes universal references work.
void section_collapsing() {
    std::cout << "\n=== Reference Collapsing (concept) ===\n";
    std::cout << "T&  + &  → T&  (any & wins)\n";
    std::cout << "T&  + && → T&  (any & wins)\n";
    std::cout << "T&& + &  → T&  (any & wins)\n";
    std::cout << "T&& + && → T&& (only && when both are &&)\n";

    // Practical demo: auto&& deduces as universal reference
    int   x = 5;
    auto&& r1 = x;    // x is lvalue → r1 deduced as int& (collapsed T& && → T&)
    auto&& r2 = 42;   // 42 is rvalue → r2 deduced as int&&
    auto&& r3 = std::move(x);  // rvalue → r3 is int&&

    std::cout << "r1 (from lvalue): " << r1 << "\n";
    std::cout << "r2 (from rvalue): " << r2 << "\n";
    std::cout << "r3 (from move):   " << r3 << "\n";
}

int main() {
    std::cout << "╔══ Topic 05: References ══╗\n";

    section_lvalue_ref();
    section_rvalue_ref();
    section_const_ref();
    section_ref_to_ptr();
    section_ref_to_array();
    section_ref_to_func();
    section_returning_ref();
    section_move();
    section_forward();
    section_collapsing();

    std::cout << "\n✓ Topic 05 complete.\n";
}
