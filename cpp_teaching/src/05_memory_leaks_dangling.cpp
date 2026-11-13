/**
 * LESSON 05 — Memory Leaks and Dangling Pointers
 * ================================================
 * Key ideas:
 *  - Memory leak: heap memory allocated but never freed.
 *  - Dangling pointer: pointer to memory that has already been freed/gone.
 *  - Use-after-free, double-free — both are Undefined Behaviour (UB).
 *  - Smart pointers + RAII eliminate virtually all these bugs.
 *  - Tools: Valgrind (Linux), AddressSanitizer (clang/gcc -fsanitize=address),
 *            Dr. Memory (Windows).
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// ── 1. Memory leak (intentional for illustration — do not do this) ───────────
void demonstrate_leak_bad() {
    // Allocate on heap, never free — this is a leak.
    int* p = new int(42);
    std::cout << "  allocated: " << *p << "\n";
    // delete p;   // ← forgotten!
    // p goes out of scope — memory is GONE (leaked) until process exits
}

void demonstrate_no_leak_good() {
    // Fix 1: use smart pointer
    auto p = std::make_unique<int>(42);
    std::cout << "  allocated (smart): " << *p << "\n";
    // p destroyed here — memory freed automatically
}

// ── 2. Dangling pointer ──────────────────────────────────────────────────────
void demonstrate_dangling_bad() {
    int* p = new int(99);
    delete p;
    // p is now DANGLING — the memory is free, but p still holds the old address
    // Accessing *p here is Undefined Behaviour:
    // std::cout << *p;   // ← DO NOT DO THIS — may crash, may print garbage

    // Fix: null the pointer after delete
    p = nullptr;
    if (p) std::cout << *p; else std::cout << "  p is null — safe.\n";
}

// ── 3. Use-after-free via raw pointer ────────────────────────────────────────
void demonstrate_uaf_bad() {
    int* p = new int(7);
    int* alias = p;     // alias also points to same memory
    delete p;
    p = nullptr;
    // alias is now dangling — alias->use is UB even though p was nulled
    // std::cout << *alias;   // ← UB, don't do

    // Fix: use shared ownership (shared_ptr) if multiple owners needed
}

void demonstrate_uaf_good() {
    auto sp1 = std::make_shared<int>(7);
    auto sp2 = sp1;         // both own the int
    sp1.reset();            // sp1 gives up ownership — int still alive (sp2 owns it)
    std::cout << "  sp2 still valid: " << *sp2 << "\n";
}   // int destroyed only when sp2 goes out of scope

// ── 4. Double-free ───────────────────────────────────────────────────────────
void demonstrate_double_free_bad() {
    int* p = new int(5);
    delete p;
    // delete p;   // ← second delete is UB (usually crash / heap corruption)

    // Fix: set to nullptr after delete, or use smart pointers
    p = nullptr;
    delete p;   // delete nullptr is well-defined (no-op) — safe
}

// ── 5. Stack dangling reference ──────────────────────────────────────────────
// NEVER return a reference or pointer to a local variable!
// const int& bad_return() {
//     int local = 42;
//     return local;    // local is destroyed — caller gets dangling ref!
// }

// Fix: return by value or return a heap-allocated resource
std::string good_return() {
    std::string local = "hello";
    return local;   // RVO or move — safe
}

// ── 6. Detecting leaks with AddressSanitizer ─────────────────────────────────
// Compile with: g++ -fsanitize=address -g lesson05.cpp
// Or with CMake: target_compile_options(target PRIVATE -fsanitize=address)
//               target_link_options(target PRIVATE -fsanitize=address)
//
// ASAN output for a leak looks like:
//   ==12345==ERROR: LeakSanitizer: detected memory leaks
//   Direct leak of 4 byte(s) in 1 object(s) allocated from:
//     #0 operator new(unsigned long) ...

// ── 7. RAII guard — the right pattern ───────────────────────────────────────
class ManagedArray {
public:
    explicit ManagedArray(std::size_t n)
        : data_(new int[n]), size_(n) {
        std::cout << "  ManagedArray alloc size=" << n << "\n";
    }
    ~ManagedArray() {
        delete[] data_;
        std::cout << "  ManagedArray freed\n";
    }
    int& operator[](std::size_t i) { return data_[i]; }

    ManagedArray(const ManagedArray&)            = delete; // no accidental copy
    ManagedArray& operator=(const ManagedArray&) = delete;

private:
    int*        data_;
    std::size_t size_;
};

// ── 8. Summary checklist ─────────────────────────────────────────────────────
void print_checklist() {
    std::cout << R"(
=== Memory Safety Checklist ===
  ✓ Every new  has a matching delete   (or use unique_ptr)
  ✓ Every new[] has a matching delete[] (or use vector)
  ✓ Set pointer to nullptr after delete
  ✓ Never return pointer/reference to local variable
  ✓ Use shared_ptr if multiple owners; weak_ptr to break cycles
  ✓ Compile with -fsanitize=address during development
  ✓ Run Valgrind on Linux, Dr. Memory on Windows
)";
}

int main() {
    std::cout << "\n=== Memory Leaks ===\n";
    demonstrate_leak_bad();         // leaks 4 bytes (acceptable in a demo)
    demonstrate_no_leak_good();

    std::cout << "\n=== Dangling Pointer ===\n";
    demonstrate_dangling_bad();

    std::cout << "\n=== Use-After-Free ===\n";
    demonstrate_uaf_bad();
    demonstrate_uaf_good();

    std::cout << "\n=== Double-Free ===\n";
    demonstrate_double_free_bad();

    std::cout << "\n=== RAII ManagedArray ===\n";
    {
        ManagedArray arr(10);
        arr[0] = 100;
        std::cout << "  arr[0]=" << arr[0] << "\n";
    }   // automatically freed

    std::string s = good_return();
    std::cout << "\n=== Safe Return: '" << s << "'\n";

    print_checklist();

    std::cout << "\n✓ Lesson 05 complete.\n";
}
