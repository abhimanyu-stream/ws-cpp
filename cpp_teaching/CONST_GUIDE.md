# `const` in C++ — Complete Guide

## Table of Contents
1. [How to Read `const` Declarations](#how-to-read-const-declarations)
2. [Basic `const` Variable](#basic-const-variable)
3. [Pointer & `const` — 4 Combinations](#pointer--const--4-combinations)
4. [References & `const`](#references--const)
5. [`const` in Functions](#const-in-functions)
6. [`const` Member Functions](#const-member-functions)
7. [`mutable` — the exception to const](#mutable--the-exception-to-const)
8. [`constexpr` — compile-time const](#constexpr--compile-time-const)
9. [`consteval` — must be compile-time (C++20)](#consteval--must-be-compile-time-c20)
10. [`constinit` — guaranteed compile-time init (C++20)](#constinit--guaranteed-compile-time-init-c20)
11. [`const` with Smart Pointers](#const-with-smart-pointers)
12. [`const` with STL Containers](#const-with-stl-containers)
13. [`const_cast` — removing const](#const_cast--removing-const)
14. [Cheat Sheet](#cheat-sheet)

---

## How to Read `const` Declarations

### The Clockwise/Spiral Rule (East const vs West const)

There are two styles people write `const`. Both are valid C++.

```cpp
const int x = 5;   // "West const" — const is on the left  (most tutorials)
int const x = 5;   // "East const" — const is on the right (more consistent)
```

They mean **exactly the same thing** for simple variables.
But for pointers, **East const** is more consistent and easier to read.

---

### The Golden Rule: **Read Right-to-Left**

Start at the variable name, go **right**, then **left**.
Say "is a", "pointer to", "const", etc.

```
int const * const ptr
          ↑       ↑
         (2)     (1)   ← read right-to-left from ptr

Step 1: ptr      → "ptr"
Step 2: const    → "is a const"
Step 3: *        → "pointer to"
Step 4: const    → "const"
Step 5: int      → "int"

Full reading: "ptr is a const pointer to const int"
```

Let's apply this to all 4 pointer combinations:

```
int * ptr              → "ptr is a pointer to int"
                          (ptr can change, *ptr can change)

int const * ptr        → "ptr is a pointer to const int"
                          (ptr can change, *ptr cannot)

int * const ptr        → "ptr is a const pointer to int"
                          (ptr cannot change, *ptr can)

int const * const ptr  → "ptr is a const pointer to const int"
                          (nothing can change)
```

---

## Basic `const` Variable

```cpp
const int x = 10;
// x = 20;          // ERROR — x is const, cannot modify

int const y = 10;   // same thing — just different style
// y = 20;          // ERROR
```

**Must be initialized** at declaration:
```cpp
const int a;        // ERROR — const variable must be initialized
const int b = 42;   // OK
```

**Use cases:**
```cpp
const int MAX_SIZE   = 1024;
const double PI      = 3.14159265358979;
const char  NEWLINE  = '\n';
const std::string PREFIX = "ERROR: ";
```

---

## Pointer & `const` — 4 Combinations

### Combination 1: Non-const pointer to non-const int
```
int * ptr
```
> "ptr is a **pointer to int**"
> Both the pointer and the pointed-to value can change.

```cpp
int a = 10, b = 20;
int* ptr = &a;

*ptr = 99;    // OK — change the value  → a is now 99
ptr  = &b;    // OK — change the pointer to point elsewhere
```

---

### Combination 2: Non-const pointer to const int
```
const int * ptr
int const * ptr   ← same thing
```
> "ptr is a **pointer to const int**"
> The **pointer** can change (point elsewhere), but you **cannot change** the value through it.

```cpp
int a = 10, b = 20;
const int* ptr = &a;

// *ptr = 99;   // ERROR — cannot modify the value through ptr
ptr = &b;       // OK — ptr itself can point to something else

// Common use: function parameters that "promise not to modify" the data
void print(const int* arr, int size) {
    for (int i = 0; i < size; i++)
        std::cout << arr[i];   // read-only — can't modify arr[i]
}
```

**Memory diagram:**
```
ptr ──────────────► [ 10 ]   (you can read 10, but can't write through ptr)
ptr = &b ─────────► [ 20 ]   (pointing elsewhere is fine)
```

---

### Combination 3: Const pointer to non-const int
```
int * const ptr
```
> "ptr is a **const pointer to int**"
> The **pointer is fixed** (always points to the same place), but you **can change** the value at that address.

```cpp
int a = 10, b = 20;
int* const ptr = &a;   // ptr is locked to &a forever

*ptr = 99;    // OK — modify the value at the address
// ptr = &b;  // ERROR — cannot change where ptr points

// Common use: 'this' pointer in C++ is essentially  T* const this
//             you can modify the object, but 'this' itself can't be reseated
```

**Memory diagram:**
```
ptr (locked) ───► [ 10 → 99 ]   (value can change, address cannot)
```

---

### Combination 4: Const pointer to const int
```
const int * const ptr
int const * const ptr   ← same thing
```
> "ptr is a **const pointer to const int**"
> **Nothing** can change — not the pointer, not the value.

```cpp
int a = 10;
const int* const ptr = &a;

// *ptr = 99;  // ERROR — can't change the value
// ptr  = &b;  // ERROR — can't change the pointer

// Common use: global lookup tables, string literals
const char* const GREETING = "Hello";
```

---

### Visual Summary Table

```
Declaration              | Change *ptr? | Change ptr? | Reading
─────────────────────────|─────────────|─────────────|──────────────────────────
int* ptr                 |     ✓ YES   |    ✓ YES    | pointer to int
const int* ptr           |     ✗ NO    |    ✓ YES    | pointer to const int
int* const ptr           |     ✓ YES   |    ✗ NO     | const pointer to int
const int* const ptr     |     ✗ NO    |    ✗ NO     | const pointer to const int
```

---

## References & `const`

A reference is already implicitly a "const pointer" — it can never be reseated.
So `T& const` is redundant and illegal.

```cpp
int a = 10, b = 20;

int& ref = a;           // lvalue reference — 'ref' is an alias for 'a'
ref = 99;               // OK — modifies 'a'
// ref = b;             // this is ASSIGNMENT (sets a=20), NOT reseating

const int& cref = a;    // const lvalue reference — read-only alias
// cref = 99;           // ERROR — cannot modify through const ref
```

### The special power of `const T&`

A const lvalue reference can bind to **anything** — lvalues AND rvalues:

```cpp
int a = 5;

const int& r1 = a;      // OK — binds to lvalue
const int& r2 = 42;     // OK — binds to rvalue (literal)
const int& r3 = a + 1;  // OK — binds to rvalue (expression)
const int& r4 = func(); // OK — binds to rvalue (function return)
```

This is why `const T&` is the standard "accept anything, read-only" parameter type:

```cpp
void log(const std::string& msg);   // accepts string, const string, string literal, temp
```

---

## `const` in Functions

### 1. `const` parameter — caller's argument won't be modified

```cpp
// Passes by const reference — read-only, no copy
void print(const std::string& s) {
    std::cout << s;
    // s = "changed";  // ERROR
}

// Passes by const pointer — read-only view of array
void display(const int* arr, int n) {
    for (int i = 0; i < n; i++)
        std::cout << arr[i];
    // arr[0] = 99;    // ERROR
}
```

### 2. `const` return value

```cpp
// Returning const value — rarely useful for primitives
const int get_max() { return 100; }

// Returning const reference — exposes internal data as read-only
class Cache {
    std::vector<int> data_;
public:
    const std::vector<int>& get() const { return data_; }
    //    ──────────────────                             ↑
    //    caller can't modify                   const member fn (see below)
};
```

---

## `const` Member Functions

A `const` after the parameter list means: **"this function does not modify the object"**.

```cpp
class Rectangle {
    double width_;
    double height_;

public:
    Rectangle(double w, double h) : width_(w), height_(h) {}

    // const member function — read-only, can be called on const objects
    double area()   const { return width_ * height_; }
    double width()  const { return width_; }

    // non-const member function — modifies the object
    void scale(double factor) {
        width_  *= factor;
        height_ *= factor;
    }
};

Rectangle r(3.0, 4.0);
r.area();      // OK — const function on non-const object
r.scale(2.0);  // OK — non-const function on non-const object

const Rectangle cr(5.0, 6.0);
cr.area();     // OK — const function on const object
// cr.scale(2.0); // ERROR — can't call non-const function on const object
```

### const overloading — two versions of the same function

You can have both a `const` and non-`const` version. The compiler picks based on whether the object is const.

```cpp
class MyString {
    char* data_;
public:
    // non-const version — returns modifiable reference
    char& operator[](int i)       { return data_[i]; }

    // const version — returns read-only reference
    const char& operator[](int i) const { return data_[i]; }
};

MyString       s  = ...;
const MyString cs = ...;

s[0]  = 'X';   // calls non-const operator[] — modifiable
// cs[0] = 'X'; // calls const operator[]    — ERROR, read-only
char c = cs[0]; // OK — reading is fine
```

---

## `mutable` — the exception to const

`mutable` allows a member to be modified even inside a `const` member function.

Use it **only** for things that don't logically affect the object's state:
- caches
- mutexes
- lazy-computed values
- debug counters

```cpp
class ExpensiveCalc {
    double input_;
    mutable double   cache_;        // mutable — can change inside const fn
    mutable bool     cached_{false};
    mutable int      call_count_{0};

public:
    explicit ExpensiveCalc(double v) : input_(v), cache_(0) {}

    double result() const {
        ++call_count_;              // OK — mutable
        if (!cached_) {
            cache_  = input_ * input_ * 3.14;  // heavy work
            cached_ = true;         // OK — mutable
        }
        return cache_;
    }

    int calls() const { return call_count_; }
};

const ExpensiveCalc ec(5.0);
std::cout << ec.result() << "\n";  // computes
std::cout << ec.result() << "\n";  // returns cached value
std::cout << ec.calls()  << "\n";  // 2
```

---

## `constexpr` — compile-time const

`constexpr` means: **"evaluate this at compile time if possible."**
Stronger than `const` — the value must be computable at compile time.

```cpp
const     int a = 10;         // runtime const — could come from user input
constexpr int b = 10;         // compile-time const — value known at compile time
constexpr int c = a * 2;      // OK — 'a' is const literal, still compile-time
// constexpr int d = cin_val; // ERROR — runtime value can't be constexpr
```

### `constexpr` functions

```cpp
constexpr int square(int x) {
    return x * x;
}

constexpr int s1 = square(5);   // evaluated at compile time → 25 baked into binary
int           s2 = square(5);   // also compile time (compiler chooses)
int           n  = get_input();
int           s3 = square(n);   // runtime (n is runtime value)
```

### `constexpr` vs `const`

```cpp
// const — the VALUE is immutable, but initialization can be runtime
const int size = get_user_input();   // OK — but can't use as array size

// constexpr — MUST be compile-time
constexpr int MAX = 100;
int arr[MAX];       // OK — compiler knows MAX is 100

// const int x = func_call(); — might or might not be compile-time
// constexpr int x = func_call(); — MUST be compile-time, error if not
```

### `constexpr` class / constructor (C++17)

```cpp
struct Point {
    double x, y;
    constexpr Point(double x, double y) : x(x), y(y) {}

    constexpr double distance_sq() const {
        return x*x + y*y;
    }
};

constexpr Point origin{0.0, 0.0};
constexpr Point p{3.0, 4.0};
constexpr double dsq = p.distance_sq();  // 25.0 — computed at compile time
```

---

## `consteval` — must be compile-time (C++20)

Stricter than `constexpr` — the function **must** be called at compile time.
Any call with a runtime argument is a **compile error**.

```cpp
consteval int cube(int x) { return x * x * x; }

constexpr int a = cube(3);    // OK — 27 at compile time
// int n = get_n();
// int b = cube(n);            // ERROR — n is runtime, consteval forbids this
```

---

## `constinit` — guaranteed compile-time init (C++20)

Ensures a variable is **initialized at compile time** but doesn't make it const.
Prevents "static initialization order fiasco."

```cpp
constinit int global_counter = 0;   // initialized at compile time
                                     // but CAN be modified at runtime
global_counter++;                    // OK — not const

// constinit const int x = 42;      // OK — both constinit + const
```

---

## `const` with Smart Pointers

Smart pointers follow the same rules as raw pointers — but the syntax looks different.

```cpp
// 1. Non-const unique_ptr to non-const int
std::unique_ptr<int> p1 = std::make_unique<int>(10);
*p1 = 99;           // OK — change value
p1  = std::make_unique<int>(20);  // OK — reseat pointer

// 2. Non-const unique_ptr to const int  (can't change value, can reseat)
std::unique_ptr<const int> p2 = std::make_unique<int>(10);
// *p2 = 99;        // ERROR — const int
p2  = std::make_unique<int>(20);  // OK — pointer can change

// 3. Const unique_ptr to non-const int  (can change value, can't reseat)
const std::unique_ptr<int> p3 = std::make_unique<int>(10);
*p3 = 99;           // OK — value can change
// p3 = ...;        // ERROR — const unique_ptr can't be reseated

// 4. Const unique_ptr to const int  (nothing can change)
const std::unique_ptr<const int> p4 = std::make_unique<int>(10);
// *p4 = 99;        // ERROR
// p4  = ...;       // ERROR

// shared_ptr — same rules
std::shared_ptr<const std::string> ro = std::make_shared<std::string>("hello");
// *ro = "changed"; // ERROR — const string
std::cout << *ro;   // OK — reading is fine
```

---

## `const` with STL Containers

```cpp
// const vector — no modification of container or elements
const std::vector<int> cv = {1, 2, 3};
// cv.push_back(4);    // ERROR — modifying container
// cv[0] = 99;         // ERROR — modifying element

for (const auto& x : cv)  // OK — read-only iteration
    std::cout << x;

// vector of const — elements are const, container can grow
std::vector<const int> vc;   // uncommon — elements can't be assigned
                              // most algorithms won't work with this

// Prefer: vector of int + const ref when passing to functions
void read_only(const std::vector<int>& v) {
    // v.push_back(4);  // ERROR
    // v[0] = 99;       // ERROR
    std::cout << v[0];  // OK
}

// const iterator vs const_iterator
std::vector<int> v = {1, 2, 3};

// const iterator — the iterator itself is const (can't advance)
const std::vector<int>::iterator it = v.begin();
*it = 99;    // OK — can modify element
// ++it;     // ERROR — iterator is const

// const_iterator — the element it points to is const (can't modify element)
std::vector<int>::const_iterator cit = v.cbegin();
// *cit = 99; // ERROR — read-only
++cit;        // OK — can advance

// C++11 shorthand:
auto cit2 = v.cbegin();   // const_iterator via cbegin()/cend()
```

---

## `const_cast` — removing const

`const_cast` removes `const` from a pointer or reference.
**Use very rarely** — only when you know the original object is not const.

```cpp
// SAFE use — the underlying object is NOT const
int value = 42;
const int* cp = &value;         // added const for some reason
int* p = const_cast<int*>(cp);  // remove const — safe, value is non-const
*p = 99;                        // OK

// UNSAFE / UNDEFINED BEHAVIOR — original object IS const
const int literal = 100;
int* danger = const_cast<int*>(&literal);
*danger = 200;   // UNDEFINED BEHAVIOR — modifying a truly const object
```

**Legitimate use case**: when calling a legacy C function that takes `char*`
but you have `const char*` and you know it won't modify the data.

```cpp
void legacy_c_func(char* s);  // old C API — doesn't take const char*

const char* msg = "hello";
legacy_c_func(const_cast<char*>(msg));  // acceptable if you're sure it won't modify
```

---

## Cheat Sheet

```
READING DECLARATIONS  — right to left from the variable name
─────────────────────────────────────────────────────────────
int x                    x is an int
const int x              x is a const int
int* p                   p is a pointer to int
const int* p             p is a pointer to const int      (*p immutable)
int* const p             p is a const pointer to int      (p immutable)
const int* const p       p is a const pointer to const int (both immutable)
int& r                   r is a reference to int
const int& r             r is a reference to const int    (read-only alias)

THE FOUR POINTER COMBINATIONS
─────────────────────────────────────────────────────────────
                         Change *p?   Change p?
int* p                      ✓            ✓
const int* p                ✗            ✓
int* const p                ✓            ✗
const int* const p          ✗            ✗

CONST KEYWORDS COMPARISON
─────────────────────────────────────────────────────────────
const        runtime const — value set once, never changes
constexpr    compile-time const — evaluated by compiler, can be used in templates/arrays
consteval    compile-time ONLY — error if called at runtime (C++20)
constinit    compile-time INIT — not const, just guarantees init order (C++20)
mutable      the escape hatch — allows modification inside const member functions

FUNCTION PARAMETER GUIDE
─────────────────────────────────────────────────────────────
void f(int x)              copy — small/trivial types
void f(const int x)        copy that you won't modify — rarely needed
void f(int& x)             modify caller's variable
void f(const int& x)       read-only, any type, no copy
void f(int* x)             nullable, modifiable pointer
void f(const int* x)       nullable, read-only pointer
void f(std::string_view x) read-only string, zero allocation (C++17)

MEMBER FUNCTION RULES
─────────────────────────────────────────────────────────────
void foo()       — can modify object, only callable on non-const objects
void foo() const — cannot modify object (except mutable members),
                   callable on BOTH const and non-const objects
                   ← always make functions const if they don't modify state
```
