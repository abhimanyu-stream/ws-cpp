/**
 * TOPIC 15 — Advanced C++ Programming
 * ======================================
 * Covers: undefined/implementation-defined behavior, object lifetime,
 *         storage duration, linkage, ODR, name mangling, vtable/vptr,
 *         RTTI, all four casts, placement new, custom allocators,
 *         cache locality, ABI basics
 *
 * Compile: g++ -std=c++17 -Wall 15_advanced_cpp.cpp -o advanced
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <typeinfo>    // typeid, type_info
#include <new>         // placement new, std::align
#include <cstddef>     // std::max_align_t
#include <cstring>     // memcpy
#include <chrono>
#include <numeric>

// ── Undefined Behavior (UB) ───────────────────────────────────────────────────
void section_ub_examples() {
    std::cout << "\n=== Undefined / Implementation-Defined Behavior ===\n";

    // UB — the standard says NOTHING about what happens
    // Compiler may delete the check, crash, do anything.
    // 1. Signed integer overflow:
    //    int x = INT_MAX; ++x;    // UB — wrap-around not guaranteed for signed
    // 2. Null pointer dereference:
    //    int* p = nullptr; *p = 1;  // UB
    // 3. Use after free:
    //    int* p = new int(1); delete p; *p = 2;  // UB
    // 4. Array out of bounds:
    //    int a[3]; a[5] = 1;      // UB
    // 5. Data race (see threading topic)

    // Implementation-defined — behavior is defined but varies by platform
    int x = -1;
    unsigned int u = x;   // well-defined: wraps per two's complement (C++20 mandates it)
    std::cout << "int -1 as unsigned: " << u << "\n";   // 4294967295 on 32-bit

    // sizeof is implementation-defined
    std::cout << "sizeof(int): "        << sizeof(int)        << "\n";   // usually 4
    std::cout << "sizeof(long):  "      << sizeof(long)       << "\n";   // 4 or 8
    std::cout << "sizeof(pointer): "    << sizeof(void*)      << "\n";   // 4 or 8

    // Unspecified — defined behavior but which one is not specified
    // int a = f() + g();  — which of f or g is called first is unspecified
}

// ── Object Lifetime and Storage Duration ─────────────────────────────────────
int global_obj = 1;        // static storage — initialized before main, destroyed after

void section_lifetime() {
    std::cout << "\n=== Object Lifetime & Storage Duration ===\n";

    // Automatic (stack) — created on declaration, destroyed at end of scope
    {
        std::string s = "automatic";
        std::cout << "auto: " << s << "\n";
    }   // s destroyed here

    // Static — created on first use (functions) or at program start (globals)
    {
        static int call_count = 0;   // initialized ONCE
        ++call_count;
        std::cout << "static call_count: " << call_count << "\n";
    }

    // Dynamic (heap) — controlled by new/delete or smart pointers
    {
        auto p = std::make_unique<std::string>("dynamic");
        std::cout << "dynamic: " << *p << "\n";
    }   // unique_ptr destructor frees heap memory

    // Thread-local — one copy per thread (C++11)
    thread_local int tl_var = 0;
    ++tl_var;
    std::cout << "thread_local: " << tl_var << "\n";
}

// ── Linkage ───────────────────────────────────────────────────────────────────
// External linkage — visible across translation units (default for non-const globals)
int   external_var = 10;        // external linkage

// Internal linkage — visible only in this translation unit
static int internal_var = 20;   // internal linkage (static at file scope)
namespace {
    int anon_var = 30;          // internal linkage via anonymous namespace (preferred)
}

// No linkage — local variables, local types

void section_linkage() {
    std::cout << "\n=== Linkage ===\n";
    std::cout << "external: " << external_var << "\n";
    std::cout << "internal: " << internal_var << "\n";
    std::cout << "anon:     " << anon_var     << "\n";
}

// ── Name Mangling ─────────────────────────────────────────────────────────────
// C++ encodes function signatures into symbol names to support overloading.
// C doesn't mangle, so C++ provides extern "C" to disable mangling for C interop.

void overloaded(int)    { std::cout << "overloaded(int)\n"; }
void overloaded(double) { std::cout << "overloaded(double)\n"; }

extern "C" void c_compatible_function() {
    // This function has no mangling — can be called from C
    std::cout << "c_compatible_function()\n";
}

void section_name_mangling() {
    std::cout << "\n=== Name Mangling ===\n";
    // typeid gives the mangled name on some compilers
    std::cout << "typeid(int): "    << typeid(int).name()    << "\n";
    std::cout << "typeid(double): " << typeid(double).name() << "\n";
    std::cout << "typeid(std::string): " << typeid(std::string).name() << "\n";
    overloaded(1);
    overloaded(1.0);
    c_compatible_function();
}

// ── vtable and vptr ───────────────────────────────────────────────────────────
// When a class has virtual functions, the compiler adds:
//   - A vtable (virtual function table) per class — array of function pointers
//   - A vptr (virtual pointer) per OBJECT — hidden pointer to class's vtable
//
// Virtual call:  obj->vptr  → vtable  → function pointer  → function body
// Cost: one extra pointer dereference + cannot be inlined by default

struct Base {
    int   data = 0;
    // vptr is hidden here — added by compiler because of virtual function

    virtual void speak() const { std::cout << "Base::speak\n"; }
    virtual void greet() const { std::cout << "Base::greet\n"; }
    virtual ~Base() = default;
};

struct Derived : Base {
    void speak() const override { std::cout << "Derived::speak\n"; }
    // greet() not overridden — Base's vtable entry used
};

void section_vtable() {
    std::cout << "\n=== vtable / vptr ===\n";
    std::cout << "sizeof(Base)   = " << sizeof(Base)    << "\n";   // includes vptr (8 bytes on 64-bit)
    std::cout << "sizeof(int)    = " << sizeof(int)     << "\n";   // just the int member

    Base    b;
    Derived d;
    Base*   p = &d;

    b.speak();   // static dispatch (no virtual needed here — object type known)
    p->speak();  // dynamic dispatch via vtable → Derived::speak
    p->greet();  // dynamic dispatch → Base::greet (not overridden in Derived)
}

// ── RTTI — Run-Time Type Information ─────────────────────────────────────────
void section_rtti() {
    std::cout << "\n=== RTTI ===\n";

    Base*    p  = new Derived();
    Derived* dp = nullptr;

    // dynamic_cast — safe downcast (requires virtual functions)
    dp = dynamic_cast<Derived*>(p);
    if (dp) std::cout << "dynamic_cast succeeded\n";

    // dynamic_cast with references throws std::bad_cast on failure
    try {
        Derived& dr = dynamic_cast<Derived&>(*p);
        (void)dr;
        std::cout << "dynamic_cast ref succeeded\n";
    }
    catch (const std::bad_cast& e) {
        std::cout << "bad_cast: " << e.what() << "\n";
    }

    // typeid — get type at runtime
    std::cout << "typeid(*p).name(): " << typeid(*p).name() << "\n";   // mangled Derived name
    std::cout << "is Derived? " << (typeid(*p) == typeid(Derived)) << "\n";

    delete p;
}

// ── The Four Casts ────────────────────────────────────────────────────────────
void section_casts() {
    std::cout << "\n=== The Four C++ Casts ===\n";

    // 1. static_cast — compile-time checked, safe conversions
    double d = 3.14;
    int    i = static_cast<int>(d);     // truncates to 3
    std::cout << "static_cast<int>(3.14) = " << i << "\n";

    // Downcast in class hierarchy (no runtime check — be sure it's correct)
    Base*    bp  = new Derived();
    Derived* dpc = static_cast<Derived*>(bp);  // no RTTI check — fast
    dpc->speak();
    delete bp;

    // 2. dynamic_cast — runtime-checked downcast (requires virtual functions + RTTI)
    Base*    bp2 = new Derived();
    Derived* dp2 = dynamic_cast<Derived*>(bp2);  // safe — returns null if fails
    if (dp2) dp2->speak();
    delete bp2;

    Base*    bp3 = new Base();
    Derived* dp3 = dynamic_cast<Derived*>(bp3);  // fails — bp3 IS a Base, not Derived
    std::cout << "dynamic_cast to wrong type: " << (dp3 == nullptr ? "null" : "ok") << "\n";
    delete bp3;

    // 3. const_cast — add or remove const (use sparingly)
    const int ci  = 100;
    const int* cp = &ci;
    // int* p = const_cast<int*>(cp);  // removes const
    // *p = 200;                        // UB — ci was originally const!
    std::cout << "const_cast: use only when original is non-const\n";

    // Safe use: calling non-const API with something you know is non-const
    int        mutable_val = 42;
    const int* cptr        = &mutable_val;
    int*       ptr         = const_cast<int*>(cptr);   // safe — mutable_val is non-const
    *ptr = 99;
    std::cout << "const_cast safe use: " << mutable_val << "\n";

    // 4. reinterpret_cast — raw bitwise reinterpretation (dangerous)
    int    n   = 0x41424344;   // 'DCBA' in memory (little-endian)
    char*  cp2 = reinterpret_cast<char*>(&n);
    std::cout << "reinterpret_cast bytes: "
              << cp2[0] << cp2[1] << cp2[2] << cp2[3] << "\n";   // DCBA on little-endian

    // Integer ↔ pointer (platform-dependent!)
    uintptr_t addr = reinterpret_cast<uintptr_t>(&n);
    int*       p2  = reinterpret_cast<int*>(addr);
    std::cout << "round-trip via uintptr_t: " << *p2 << "\n";
}

// ── Placement new ─────────────────────────────────────────────────────────────
// Construct an object at a specific memory location (no allocation).
// Used in memory pools, embedded systems, ring buffers.
void section_placement_new() {
    std::cout << "\n=== Placement new ===\n";

    // Allocate raw aligned memory
    alignas(std::string) char buf[sizeof(std::string)];

    // Construct in-place — no heap allocation
    std::string* p = new (buf) std::string("placement new!");

    std::cout << *p << "\n";

    // Must manually call destructor (placement new doesn't pair with delete)
    p->~basic_string();   // explicit destructor call

    // With a pre-allocated array — simple memory pool sketch
    constexpr int POOL_SIZE = 4;
    alignas(int) char pool[sizeof(int) * POOL_SIZE];

    int* arr[POOL_SIZE];
    for (int i = 0; i < POOL_SIZE; ++i) {
        arr[i] = new (pool + i * sizeof(int)) int(i * 10);
    }

    for (int i = 0; i < POOL_SIZE; ++i)
        std::cout << "pool[" << i << "] = " << *arr[i] << "\n";

    // No delete[] needed for placement new — raw memory freed when pool goes out of scope
}

// ── Cache Locality ────────────────────────────────────────────────────────────
// Modern CPUs have L1/L2/L3 caches. Accessing memory sequentially is fast
// (cache line prefetching). Random access = cache miss = slow.

void section_cache_locality() {
    std::cout << "\n=== Cache Locality ===\n";

    constexpr int N = 1024;

    // Row-major access — fast (accesses memory sequentially per cache line)
    std::vector<std::vector<int>> matrix(N, std::vector<int>(N, 1));

    auto t1 = std::chrono::high_resolution_clock::now();
    long long sum1 = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            sum1 += matrix[i][j];     // row-major — cache-friendly
    auto t2 = std::chrono::high_resolution_clock::now();

    // Column-major access — slow (jumps N*sizeof(int) bytes per step)
    long long sum2 = 0;
    auto t3 = std::chrono::high_resolution_clock::now();
    for (int j = 0; j < N; ++j)
        for (int i = 0; i < N; ++i)
            sum2 += matrix[i][j];     // column-major — cache-unfriendly
    auto t4 = std::chrono::high_resolution_clock::now();

    auto ms1 = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
    auto ms2 = std::chrono::duration_cast<std::chrono::microseconds>(t4-t3).count();

    std::cout << "Row-major    (cache friendly): " << ms1 << " μs\n";
    std::cout << "Column-major (cache hostile):  " << ms2 << " μs\n";
    std::cout << "sums: " << sum1 << " " << sum2 << " (must match)\n";

    // Array of Structs vs Struct of Arrays
    struct Particle_AOS { float x, y, z, w; };   // AoS — may have cache issues
    struct ParticleSOA {                           // SoA — better for SIMD/cache
        std::vector<float> x, y, z, w;
    };
    std::cout << "AoS vs SoA: prefer SoA for large data processed in bulk\n";
}

// ── Memory Alignment and Padding ─────────────────────────────────────────────
void section_alignment() {
    std::cout << "\n=== Memory Alignment & Padding ===\n";

    // Struct padding — compiler adds padding to satisfy alignment requirements
    struct Padded {
        char   a;      // 1 byte
        // 3 bytes padding here (to align 'b' on 4-byte boundary)
        int    b;      // 4 bytes
        char   c;      // 1 byte
        // 7 bytes padding here (to align 'd' on 8-byte boundary)
        double d;      // 8 bytes
        // Total: 1+3+4+1+7+8 = 24 bytes
    };

    struct Packed {
        double d;      // 8 bytes first (largest)
        int    b;      // 4 bytes
        char   a;      // 1 byte
        char   c;      // 1 byte
        // 2 bytes padding at end to align next struct instance
        // Total: 8+4+1+1+2 = 16 bytes
    };

    std::cout << "sizeof(Padded): " << sizeof(Padded) << " (with padding)\n";
    std::cout << "sizeof(Packed): " << sizeof(Packed) << " (reordered)\n";

    // alignas — specify alignment
    alignas(64) char cache_line[64];   // aligned to cache line boundary
    std::cout << "cache_line alignment: " << alignof(decltype(cache_line)) << "\n";

    // alignof — query alignment requirement
    std::cout << "alignof(char):   "   << alignof(char)   << "\n";
    std::cout << "alignof(int):    "   << alignof(int)    << "\n";
    std::cout << "alignof(double): "   << alignof(double) << "\n";

    (void)cache_line;
}

int main() {
    std::cout << "╔══ Topic 15: Advanced C++ ══╗\n";

    section_ub_examples();
    section_lifetime();
    section_linkage();
    section_name_mangling();
    section_vtable();
    section_rtti();
    section_casts();
    section_placement_new();
    section_cache_locality();
    section_alignment();

    std::cout << "\n✓ Topic 15 complete.\n";
}
