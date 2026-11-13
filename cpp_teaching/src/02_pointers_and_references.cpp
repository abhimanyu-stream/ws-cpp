/**
 * LESSON 02 — Pointers vs References
 * ====================================
 * Key ideas:
 *  - A pointer holds an address; it can be null, reassigned, and arithmetic works on it.
 *  - A reference is an alias; it must bind at declaration, cannot be null, cannot rebind.
 *  - Prefer references for simple aliases; prefer pointers when nullable/rebindable.
 *  - const correctness: const T*, T* const, const T* const.
 */

#include <iostream>
#include <string>
#include <cassert>

// ── 1. Basic pointer vs reference ───────────────────────────────────────────
void basics() {
    std::cout << "\n=== Basics ===\n";

    int x = 10, y = 20;

    // --- Pointer ---
    int* ptr = &x;          // ptr holds address of x
    std::cout << "ptr points to x=" << *ptr << "  addr=" << ptr << "\n";
    ptr = &y;               // pointer can be re-seated
    std::cout << "ptr now points to y=" << *ptr << "\n";
    ptr = nullptr;          // pointer can be null

    // --- Reference ---
    int& ref = x;           // ref is an alias for x
    std::cout << "ref=" << ref << "  &ref=" << &ref << "  &x=" << &x << "\n";
    // ref = y;             // this ASSIGNS y's VALUE to x, does NOT rebind ref
    ref = y;
    std::cout << "after ref=y:  x=" << x << " (x was modified, ref still aliases x)\n";
}

// ── 2. const variations ──────────────────────────────────────────────────────
void const_pointers() {
    std::cout << "\n=== const Pointer Variations ===\n";

    int a = 1, b = 2;

    const int* p1 = &a;         // pointer to const int — data is read-only
    // *p1 = 99;                // ✗ compile error
    p1 = &b;                    // ✓ pointer itself can change
    std::cout << "p1 (ptr-to-const): *p1=" << *p1 << "\n";

    int* const p2 = &a;         // const pointer to int — pointer is fixed
    *p2 = 99;                   // ✓ data can change
    // p2 = &b;                 // ✗ compile error
    std::cout << "p2 (const-ptr): *p2=" << *p2 << "  a=" << a << "\n";

    const int* const p3 = &a;   // const pointer to const int — both fixed
    // *p3 = 1; p3 = &b;        // ✗ both errors
    std::cout << "p3 (const-ptr-to-const): *p3=" << *p3 << "\n";
}

// ── 3. Passing to functions ──────────────────────────────────────────────────
void by_value(int v)        { v *= 2; }         // copy — caller unchanged
void by_pointer(int* p)     { if(p) *p *= 2; }  // modifies via pointer
void by_reference(int& r)   { r *= 2; }          // modifies original
void by_const_ref(const std::string& s) {        // read-only, no copy
    std::cout << "  s.size()=" << s.size() << "\n";
}

void function_params() {
    std::cout << "\n=== Function Parameter Passing ===\n";

    int n = 5;
    by_value(n);    std::cout << "after by_value:   n=" << n << "\n";   // 5
    by_pointer(&n); std::cout << "after by_pointer: n=" << n << "\n";  // 10
    by_reference(n);std::cout << "after by_ref:     n=" << n << "\n";  // 20

    std::string s(1'000, 'x');
    by_const_ref(s);    // no copy of the 1000-char string
}

// ── 4. Pointer arithmetic ────────────────────────────────────────────────────
void pointer_arithmetic() {
    std::cout << "\n=== Pointer Arithmetic ===\n";

    int arr[] = {10, 20, 30, 40, 50};
    int* p = arr;               // p points to arr[0]

    std::cout << "p[0]=" << *p << "\n";
    ++p;                        // advance by sizeof(int)
    std::cout << "after ++p: *p=" << *p << " (arr[1])\n";
    std::cout << "p[2]=" << p[2] << " (arr[3])\n";

    int* end = arr + 5;         // one-past-the-end (valid, do not dereference)
    std::cout << "distance end-p=" << (end - p) << "\n";
}

// ── 5. Dangling reference (conceptual — do NOT do this) ──────────────────────
// int& bad_ref() {
//     int local = 42;
//     return local;   // UB: returns ref to destroyed local
// }

// ── 6. nullptr safety ────────────────────────────────────────────────────────
void null_safety() {
    std::cout << "\n=== nullptr Safety ===\n";

    int* p = nullptr;
    if (p != nullptr) {
        std::cout << *p << "\n";
    } else {
        std::cout << "Pointer is null — safe check prevents UB.\n";
    }

    // References can NEVER be null — that's their safety guarantee.
}

int main() {
    basics();
    const_pointers();
    function_params();
    pointer_arithmetic();
    null_safety();

    std::cout << "\n✓ Lesson 02 complete.\n";
}
