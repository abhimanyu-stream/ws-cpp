/**
 * TOPIC 03 — Pointers — Complete Coverage
 * =========================================
 * Covers: declaration, arithmetic, null/void/wild/dangling,
 *         pointer-to-pointer, const variants, arrays/strings,
 *         dynamic memory, memory leaks, shallow vs deep copy
 *
 * Compile: g++ -std=c++17 -Wall 03_pointers.cpp -o pointers
 */

#include <iostream>
#include <cstring>   // strcpy, strlen
#include <cassert>

// ── Basic Pointers ────────────────────────────────────────────────────────────
void section_basics() {
    std::cout << "\n=== Pointer Basics ===\n";

    int x = 42;

    // & — address-of operator: gives the memory address of x
    // * — dereference operator: accesses the value at an address

    int* p = &x;   // p holds the address of x

    std::cout << "x     = " << x  << "\n";      // value: 42
    std::cout << "&x    = " << &x << "\n";      // address (hex)
    std::cout << "p     = " << p  << "\n";      // same address
    std::cout << "*p    = " << *p << "\n";      // dereference: 42

    *p = 99;    // modify x through the pointer
    std::cout << "x after *p=99: " << x << "\n";   // 99
}

// ── nullptr, Null Pointers, Wild & Dangling ───────────────────────────────────
void section_null_wild_dangling() {
    std::cout << "\n=== null / wild / dangling ===\n";

    // nullptr (C++11) — the type-safe null pointer constant
    int* safe = nullptr;
    if (safe == nullptr)
        std::cout << "safe ptr is null\n";

    // Wild pointer — declared but NOT initialized → undefined behavior
    // int* wild;      // points to random memory — NEVER dereference!
    // *wild = 5;      // UB — could crash, corrupt memory, etc.
    // Always initialize pointers: int* wild = nullptr;

    // Dangling pointer — points to memory that was freed
    int* dangling = new int(10);
    delete dangling;
    // *dangling = 5;  // UB — memory was freed, don't use!
    dangling = nullptr;  // good practice: null it after delete
    std::cout << "dangling reset to nullptr\n";

    // Void pointer — points to any type, must cast before dereferencing
    int    n = 100;
    void*  vp = &n;
    int*   ip = static_cast<int*>(vp);  // must cast to use
    std::cout << "void ptr → int: " << *ip << "\n";
}

// ── Pointer Arithmetic ────────────────────────────────────────────────────────
// When you increment a pointer, it advances by sizeof(T) bytes.
void section_arithmetic() {
    std::cout << "\n=== Pointer Arithmetic ===\n";

    int arr[] = {10, 20, 30, 40, 50};
    int* p    = arr;    // points to arr[0]

    std::cout << "*p     = " << *p     << " (arr[0])\n";   // 10
    std::cout << "*(p+1) = " << *(p+1) << " (arr[1])\n";   // 20
    std::cout << "*(p+4) = " << *(p+4) << " (arr[4])\n";   // 50

    ++p;    // advance to arr[1]
    std::cout << "after ++p, *p = " << *p << "\n";   // 20

    // Pointer subtraction — gives number of elements between them
    int* first = arr;
    int* last  = arr + 4;
    std::cout << "distance: " << (last - first) << " elements\n";   // 4

    // Pointer comparison
    std::cout << "(first < last): " << (first < last) << "\n";   // 1 (true)
}

// ── Pointer to Pointer ────────────────────────────────────────────────────────
void section_ptr_to_ptr() {
    std::cout << "\n=== Pointer to Pointer ===\n";

    int   val  = 42;
    int*  p    = &val;    // pointer to int
    int** pp   = &p;      // pointer to pointer to int
    int*** ppp = &pp;     // triple pointer

    std::cout << "val    = " << val    << "\n";
    std::cout << "*p     = " << *p     << "\n";
    std::cout << "**pp   = " << **pp   << "\n";
    std::cout << "***ppp = " << ***ppp << "\n";

    // Common use case: modify a pointer from a function
    **pp = 100;    // changes val through two levels of indirection
    std::cout << "val after **pp=100: " << val << "\n";   // 100

    // 2D dynamic array uses int** — see dynamic memory section
}

// ── Const Pointer Variants ────────────────────────────────────────────────────
void section_const_pointers() {
    std::cout << "\n=== Const Pointer Variants ===\n";

    int a = 10, b = 20;

    // 1. Pointer to const int — can change WHERE it points, not WHAT it points to
    const int* p1 = &a;
    p1 = &b;        // OK — pointer can change
    // *p1 = 99;    // ERROR — value is const
    std::cout << "ptr-to-const: *p1=" << *p1 << "\n";

    // 2. Const pointer to int — cannot change WHERE it points, but CAN change value
    int* const p2 = &a;
    *p2 = 99;       // OK — value can change
    // p2 = &b;     // ERROR — pointer is const
    std::cout << "const-ptr: a=" << a << " (modified through const ptr)\n";

    // 3. Const pointer to const int — NOTHING can change
    const int* const p3 = &b;
    // *p3 = 1;     // ERROR
    // p3  = &a;    // ERROR
    std::cout << "const-ptr-to-const: *p3=" << *p3 << "\n";
}

// ── Pointers and Arrays ───────────────────────────────────────────────────────
void section_arrays() {
    std::cout << "\n=== Pointers & Arrays ===\n";

    int arr[] = {1, 2, 3, 4, 5};

    // Array name decays to pointer to its first element
    int* p = arr;    // same as &arr[0]

    // These are equivalent:
    std::cout << "arr[2]    = " << arr[2]    << "\n";   // 3
    std::cout << "*(arr+2)  = " << *(arr+2)  << "\n";   // 3
    std::cout << "p[2]      = " << p[2]      << "\n";   // 3
    std::cout << "*(p+2)    = " << *(p+2)    << "\n";   // 3

    // Array of pointers
    int x = 10, y = 20, z = 30;
    int* ptrs[] = { &x, &y, &z };
    for (auto* ptr : ptrs)
        std::cout << *ptr << " ";
    std::cout << "\n";

    // Pointer to array — fixed-size type
    int (*parr)[5] = &arr;   // pointer to array of 5 ints
    std::cout << "(*parr)[3] = " << (*parr)[3] << "\n";   // 4
}

// ── Pointers and Strings ──────────────────────────────────────────────────────
void section_strings() {
    std::cout << "\n=== Pointers & Strings ===\n";

    // C-style string — array of chars ending in '\0'
    char str[] = "Hello";       // mutable, stored on stack
    const char* lit = "World";  // string literal — stored in read-only memory

    std::cout << str  << "\n";
    std::cout << lit  << "\n";

    // Pointer traversal of a string
    const char* p = str;
    while (*p != '\0') {
        std::cout << *p++ << "-";
    }
    std::cout << "\n";

    // String manipulation with pointers
    char buf[20];
    std::strcpy(buf, str);
    std::cout << "strcpy: " << buf << "\n";
    std::cout << "strlen: " << std::strlen(str) << "\n";
}

// ── Pointers and Structures ───────────────────────────────────────────────────
struct Point {
    int x, y;
    void print() const {
        std::cout << "(" << x << ", " << y << ")\n";
    }
};

void section_structs() {
    std::cout << "\n=== Pointers & Structs ===\n";

    Point pt{3, 4};
    Point* pp = &pt;

    // Two ways to access members through a pointer:
    std::cout << "(*pp).x = " << (*pp).x << "\n";   // dereference then member
    std::cout << "pp->x   = " << pp->x   << "\n";   // arrow operator (shorthand)

    pp->x = 10;
    pp->print();   // (10, 4)
}

// ── Dynamic Memory Allocation ─────────────────────────────────────────────────
void section_dynamic_memory() {
    std::cout << "\n=== Dynamic Memory ===\n";

    // new — allocate on heap, returns a pointer
    int* p = new int(42);
    std::cout << "heap int: " << *p << "\n";
    delete p;       // free the memory
    p = nullptr;    // prevent dangling

    // new[] — allocate array on heap
    int n = 5;
    int* arr = new int[n];
    for (int i = 0; i < n; ++i) arr[i] = i * i;
    for (int i = 0; i < n; ++i) std::cout << arr[i] << " ";
    std::cout << "\n";
    delete[] arr;   // must use delete[] for arrays
    arr = nullptr;

    // 2D dynamic array
    int rows = 3, cols = 4;
    int** matrix = new int*[rows];
    for (int i = 0; i < rows; ++i)
        matrix[i] = new int[cols];

    // Fill
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            matrix[i][j] = i * cols + j;

    // Print
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j)
            std::cout << matrix[i][j] << "\t";
        std::cout << "\n";
    }

    // Free 2D array — must free in reverse order
    for (int i = 0; i < rows; ++i)
        delete[] matrix[i];
    delete[] matrix;
}

// ── Memory Leaks ─────────────────────────────────────────────────────────────
// A memory leak happens when you allocate heap memory but never free it.
// The OS reclaims memory when the process exits, but long-running programs
// can run out of memory from leaks.
void section_memory_leak_demo() {
    std::cout << "\n=== Memory Leak Demo ===\n";

    // BAD — leak: p is reassigned without freeing old memory
    int* p = new int(10);
    // p = new int(20);     // LEAK — first allocation lost forever
    // delete p;            // only frees the second allocation

    // GOOD — always pair new with delete
    int* q = new int(10);
    delete q;
    q = nullptr;

    std::cout << "No leak in this demo (using RAII / smart ptrs preferred)\n";
}

// ── Shallow Copy vs Deep Copy ─────────────────────────────────────────────────
struct ShallowArray {
    int*  data;
    int   size;

    ShallowArray(int n) : size(n) {
        data = new int[n];
        for (int i = 0; i < n; ++i) data[i] = i;
    }

    // Default copy constructor does SHALLOW copy — copies the POINTER, not data
    // Both objects then share the same heap memory — double-free on destruction!
    ~ShallowArray() { delete[] data; }
};

struct DeepArray {
    int*  data;
    int   size;

    DeepArray(int n) : size(n) {
        data = new int[n];
        for (int i = 0; i < n; ++i) data[i] = i;
    }

    // Deep copy constructor — allocates NEW memory and copies values
    DeepArray(const DeepArray& other) : size(other.size) {
        data = new int[size];
        std::copy(other.data, other.data + size, data);
    }

    ~DeepArray() { delete[] data; }
};

void section_copy() {
    std::cout << "\n=== Shallow vs Deep Copy ===\n";

    // Deep copy — safe
    DeepArray a(5);
    DeepArray b = a;        // deep copy — b has its OWN memory
    b.data[0] = 999;        // doesn't affect 'a'
    std::cout << "a[0]=" << a.data[0] << " b[0]=" << b.data[0] << "\n"; // 0, 999

    // ShallowArray c(5);
    // ShallowArray d = c;  // shallow — d.data == c.data (SAME pointer!)
    //                      // when c and d destruct, delete[] is called TWICE → crash
}

int main() {
    std::cout << "╔══ Topic 03: Pointers ══╗\n";

    section_basics();
    section_null_wild_dangling();
    section_arithmetic();
    section_ptr_to_ptr();
    section_const_pointers();
    section_arrays();
    section_strings();
    section_structs();
    section_dynamic_memory();
    section_memory_leak_demo();
    section_copy();

    std::cout << "\n✓ Topic 03 complete.\n";
}
